//
// Created by Administrator on 2020/12/29 0029.
//

#include <log.h>
#include "OverScroller.h"
#include "input/GestureContext.h"

#if BX_COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
#endif

namespace h7{

    float OverScroller::SplineOverScroller::getDeceleration(int velocity) {
        return velocity > 0 ? -GRAVITY : GRAVITY;
    }

    void OverScroller::SplineOverScroller::adjustDuration(int start, int oldFinal, int newFinal) {
        const int oldDistance = oldFinal - start;
        const int newDistance = newFinal - start;
        const float x = bx::abs((float) newDistance / oldDistance);
        const int index = (int) (NB_SAMPLES * x);
        if (index < NB_SAMPLES) {
            const float x_inf = (float) index / NB_SAMPLES;
            const float x_sup = (float) (index + 1) / NB_SAMPLES;
            const float t_inf = SPLINE_TIME[index];
            const float t_sup = SPLINE_TIME[index + 1];
            const float timeCoef = t_inf + (x - x_inf) / (x_sup - x_inf) * (t_sup - t_inf);
            mDuration *= timeCoef;
        }
    }
    double OverScroller::SplineOverScroller::getSplineDeceleration(int velocity) {
        return bx::log(INFLEXION * bx::abs(velocity) / (mFlingFriction * mPhysicalCoeff));
    }
    double OverScroller::SplineOverScroller::getSplineFlingDistance(int velocity){
        double l = getSplineDeceleration(velocity);
        double decelMinusOne = DECELERATION_RATE - 1.0;
        return mFlingFriction * mPhysicalCoeff * bx::exp(DECELERATION_RATE / decelMinusOne * l);
    }
    int OverScroller::SplineOverScroller::getSplineFlingDuration(int velocity) {
        double l = getSplineDeceleration(velocity);
        double decelMinusOne = DECELERATION_RATE - 1.0;
        return (int) (1000.0 * bx::exp(l / decelMinusOne));
    }
    void OverScroller::SplineOverScroller::fitOnBounceCurve(int start, int end, int velocity) {
        // Simulate a bounce that started from edge
        const float durationToApex = - velocity / mDeceleration;
        // The float cast below is necessary to avoid integer overflow.
        const float velocitySquared = (float) velocity * velocity;
        const float distanceToApex = velocitySquared / 2.0f / bx::abs(mDeceleration);
        const float distanceToEdge = bx::abs(end - start);
        const auto totalDuration = (float) bx::sqrt(
                2.0 * (distanceToApex + distanceToEdge) / bx::abs(mDeceleration));
        mStartTime -= (int) (1000.0f * (totalDuration - durationToApex));
        mCurrentPosition = mStart = end;
        mVelocity = (int) (- mDeceleration * totalDuration);
    }

    void OverScroller::SplineOverScroller::startSpringback(int start, int end, int velocity) {
        // mStartTime has been set
        mFinished = false;
        mState = CUBIC;
        mCurrentPosition = mStart = start;
        mFinal = end;
        const int delta = start - end;
        mDeceleration = getDeceleration(delta);
        // TODO take velocity into account
        mVelocity = -delta; // only sign is used
        mOver = bx::abs(delta);
        mDuration = (int) (1000.0 * bx::sqrt(-2.0 * delta / mDeceleration));
    }
    void OverScroller::SplineOverScroller::startBounceAfterEdge(int start, int end, int velocity) {
        mDeceleration = getDeceleration(velocity == 0 ? start - end : velocity);
        fitOnBounceCurve(start, end, velocity);
        onEdgeReached();
    }
    void OverScroller::SplineOverScroller::startAfterEdge(int start, int min, int max,
                                                          int velocity) {
        if (start > min && start < max) {
            LOGE("OverScroller: %s", "startAfterEdge called from a valid position");
            mFinished = true;
            return;
        }
        const bool positive = start > max;
        const int edge = positive ? max : min;
        const int overDistance = start - edge;
        bool keepIncreasing = overDistance * velocity >= 0;
        if (keepIncreasing) {
            // Will result in a bounce or a to_boundary depending on velocity.
            startBounceAfterEdge(start, edge, velocity);
        } else {
            const double totalDistance = getSplineFlingDistance(velocity);
            if (totalDistance > bx::abs(overDistance)) {
                fling(start, velocity, positive ? min : start, positive ? start : max, mOver);
            } else {
                startSpringback(start, edge, velocity);
            }
        }
    }
    void OverScroller::SplineOverScroller::notifyEdgeReached(int start, int end, int over) {
        // mState is used to detect successive notifications
        if (mState == SPLINE) {
            mOver = over;
            mStartTime = getCurrentTimeInMsec();
            // We were in fling/scroll mode before: current velocity is such that distance to
            // edge is increasing. This ensures that startAfterEdge will not start a new fling.
            startAfterEdge(start, end, end, (int) mCurrVelocity);
        }
    }
    void OverScroller::SplineOverScroller::onEdgeReached() {
        // mStart, mVelocity and mStartTime were adjusted to their values when edge was reached.
        // The float cast below is necessary to avoid integer overflow.
        const float velocitySquared = (float) mVelocity * mVelocity;
        float distance = velocitySquared / (2.0f * bx::abs(mDeceleration));
        const float sign = bx::sign(mVelocity);//math.signum

        if (distance > mOver) {
            // Default deceleration is not sufficient to slow us down before boundary
            mDeceleration = - sign * velocitySquared / (2.0f * mOver);
            distance = mOver;
        }

        mOver = (int) distance;
        mState = BALLISTIC;
        mFinal = mStart + (int) (mVelocity > 0 ? distance : -distance);
        mDuration = - (int) (1000.0f * mVelocity / mDeceleration);
    }
    void OverScroller::SplineOverScroller::init() {
        DECELERATION_RATE = (float) (bx::log(0.78) / bx::log(0.9));
        float x_min = 0.0f;
        float y_min = 0.0f;
        for (int i = 0; i < NB_SAMPLES; i++) {
            const float alpha = (float) i / NB_SAMPLES;

            float x_max = 1.0f;
            float x, tx, coef;
            while (true) {
                x = x_min + (x_max - x_min) / 2.0f;
                coef = 3.0f * x * (1.0f - x);
                tx = coef * ((1.0f - x) * P1 + x * P2) + x * x * x;
                if (bx::abs(tx - alpha) < 1E-5) break;
                if (tx > alpha) x_max = x;
                else x_min = x;
            }
            SPLINE_POSITION[i] = coef * ((1.0f - x) * START_TENSION + x) + x * x * x;

            float y_max = 1.0f;
            float y, dy;
            while (true) {
                y = y_min + (y_max - y_min) / 2.0f;
                coef = 3.0f * y * (1.0f - y);
                dy = coef * ((1.0f - y) * START_TENSION + y) + y * y * y;
                if (bx::abs(dy - alpha) < 1E-5) break;
                if (dy > alpha) y_max = y;
                else y_min = y;
            }
            SPLINE_TIME[i] = coef * ((1.0f - y) * P1 + y * P2) + y * y * y;
        }
        SPLINE_POSITION[NB_SAMPLES] = SPLINE_TIME[NB_SAMPLES] = 1.0f;
    }
    OverScroller::SplineOverScroller::SplineOverScroller(h7::Context &context) {
        mFinished = true;
       // float ppi = context.getResources().getDisplayMetrics().density * 160.0f;
        mPhysicalCoeff = ViewConfiguration::GRAVITY_EARTH // g (m/s^2)
                         * 39.37f // inch/meter
                         * context.displayInfo.ppi
                         * 0.84f; // look and feel tuning
    }
    void OverScroller::SplineOverScroller::setFriction(float friction) {
        OverScroller::SplineOverScroller::mFlingFriction = friction;
    }
    void OverScroller::SplineOverScroller::updateScroll(float q) {
        mCurrentPosition = mStart + static_cast<int>(bx::round(q * (mFinal - mStart)));
    }
    void OverScroller::SplineOverScroller::startScroll(int start, int distance, int duration) {
        mFinished = false;

        mCurrentPosition = mStart = start;
        mFinal = start + distance;

        mStartTime = getCurrentTimeInMsec();
        mDuration = duration;

        // Unused
        mDeceleration = 0.0f;
        mVelocity = 0;
    }
    void OverScroller::SplineOverScroller::finish() {
        mCurrentPosition = mFinal;
        // Not reset since WebView relies on this value for fast fling.
        // TODO: restore when WebView uses the fast fling implemented in this class.
        // mCurrVelocity = 0.0f;
        mFinished = true;
    }
    void OverScroller::SplineOverScroller::setFinalPosition(int position) {
        mFinal = position;
        mFinished = false;
    }
    void OverScroller::SplineOverScroller::extendDuration(int extend) {
        const long time = getCurrentTimeInMsec();
        const int elapsedTime = (int) (time - mStartTime);
        mDuration = elapsedTime + extend;
        mFinished = false;
    }

    bool OverScroller::SplineOverScroller::springback(int start, int min, int max) {
        mFinished = true;

        mCurrentPosition = mStart = mFinal = start;
        mVelocity = 0;

        mStartTime = getCurrentTimeInMsec();
        mDuration = 0;

        if (start < min) {
            startSpringback(start, min, 0);
        } else if (start > max) {
            startSpringback(start, max, 0);
        }
        return !mFinished;
    }
    void OverScroller::SplineOverScroller::fling(int start, int velocity, int min, int max,
                                                 int over) {
        mOver = over;
        mFinished = false;
        mCurrVelocity = mVelocity = velocity;
        mDuration = mSplineDuration = 0;
        mStartTime = getCurrentTimeInMsec();
        mCurrentPosition = mStart = start;

        if (start > max || start < min) {
            startAfterEdge(start, min, max, velocity);
            return;
        }

        mState = SPLINE;
        double totalDistance = 0.0;

        if (velocity != 0) {
            mDuration = mSplineDuration = getSplineFlingDuration(velocity);
            totalDistance = getSplineFlingDistance(velocity);
        }

        mSplineDistance = (int) (totalDistance * bx::sign(velocity)); //math.signum
        mFinal = start + mSplineDistance;

        // Clamp to a valid final position
        if (mFinal < min) {
            adjustDuration(mStart, mFinal, min);
            mFinal = min;
        }

        if (mFinal > max) {
            adjustDuration(mStart, mFinal, max);
            mFinal = max;
        }
    }
    bool OverScroller::SplineOverScroller::continueWhenFinished() {
        switch (mState) {
            case SPLINE:
                // Duration from start to null velocity
                if (mDuration < mSplineDuration) {
                    // If the animation was clamped, we reached the edge
                    mCurrentPosition = mStart = mFinal;
                    // TODO Better compute speed when edge was reached
                    mVelocity = (int) mCurrVelocity;
                    mDeceleration = getDeceleration(mVelocity);
                    mStartTime += mDuration;
                    onEdgeReached();
                } else {
                    // Normal stop, no need to continue
                    return false;
                }
                break;
            case BALLISTIC:
                mStartTime += mDuration;
                startSpringback(mFinal, mStart, 0);
                break;
            case CUBIC:
                return false;
        }
        update();
        return true;
    }
    bool OverScroller::SplineOverScroller::update() {
        const auto time = getCurrentTimeInMsec();
        const auto currentTime = time - mStartTime;

        if (currentTime == 0) {
            // Skip work but report that we're still going if we have a nonzero duration.
            return mDuration > 0;
        }
        if (currentTime > mDuration) {
            return false;
        }

        double distance = 0.0;
        switch (mState) {
            case SPLINE: {
                const float t = (float) currentTime / mSplineDuration;
                const int index = (int) (NB_SAMPLES * t);
                float distanceCoef = 1.f;
                float velocityCoef = 0.f;
                if (index < NB_SAMPLES) {
                    const float t_inf = (float) index / NB_SAMPLES;
                    const float t_sup = (float) (index + 1) / NB_SAMPLES;
                    const float d_inf = SPLINE_POSITION[index];
                    const float d_sup = SPLINE_POSITION[index + 1];
                    velocityCoef = (d_sup - d_inf) / (t_sup - t_inf);
                    distanceCoef = d_inf + (t - t_inf) * velocityCoef;
                }

                distance = distanceCoef * mSplineDistance;
                mCurrVelocity = velocityCoef * mSplineDistance / mSplineDuration * 1000.0f;
                break;
            }

            case BALLISTIC: {
                const float t = currentTime / 1000.0f;
                mCurrVelocity = mVelocity + mDeceleration * t;
                distance = mVelocity * t + mDeceleration * t * t / 2.0f;
                break;
            }

            case CUBIC: {
                const float t = (float) (currentTime) / mDuration;
                const float t2 = t * t;
                const float sign = bx::sign(mVelocity); //Math.signum
                distance = sign * mOver * (3.0f * t2 - 2.0f * t * t2);
                mCurrVelocity = sign * mOver * 6.0f * (- t + t2);
                break;
            }
        }

        mCurrentPosition = mStart + (int) bx::round(distance);
        return true;
    }

}
