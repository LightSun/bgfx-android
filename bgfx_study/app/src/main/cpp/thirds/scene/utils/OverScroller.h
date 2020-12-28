//
// Created by Administrator on 2020/12/28 0028.
//

#ifndef BGFX_STUDY_OVERSCROLLER_H
#define BGFX_STUDY_OVERSCROLLER_H

#include <application/Context.h>
#include "../Interpolation.h"
#include "../../application/ViewConfiguration.h"

namespace h7 {

    class OverScroller {

    public:
        OverScroller();
    private:
        static constexpr int SPLINE = 0;
        static constexpr int CUBIC = 1;
        static constexpr int BALLISTIC = 2;

        unsigned char mMode;

        Interpolation *mInterpolator;

        bool mFlywheel;

        static constexpr int DEFAULT_DURATION = 250;
        static constexpr unsigned char SCROLL_MODE = 0;
        static constexpr unsigned char FLING_MODE = 1;

        class SplineOverScroller {
        private:
            // Initial position
            int mStart;

            // Current position
            int mCurrentPosition;

            // Final position
            int mFinal;

            // Initial velocity
            int mVelocity;

            // Current velocity
            float mCurrVelocity;

            // Constant current deceleration
            float mDeceleration;

            // Animation starting time, in system milliseconds
            long long mStartTime;

            // Animation duration, in milliseconds
            int mDuration;

            // Duration to complete spline component of animation
            int mSplineDuration;

            // Distance to travel along spline animation
            int mSplineDistance;

            // Whether the animation is currently in progress
            bool mFinished;

            // The allowed overshot distance before boundary is reached.
            int mOver;

            // Fling friction
            float mFlingFriction = ViewConfiguration::getScrollFriction();

            // Current state of the animation.
            int mState = SPLINE;

            // Constant gravity value, used in the deceleration phase.
            static constexpr float GRAVITY = 2000.0f;

            // A context-specific coefficient adjusted to physical values.
            float mPhysicalCoeff;

            static float DECELERATION_RATE;
            static constexpr float INFLEXION = 0.35f; // Tension lines cross at (INFLEXION, 1)
            static constexpr float START_TENSION = 0.5f;
            static constexpr float END_TENSION = 1.0f;
            static constexpr float P1 = START_TENSION * INFLEXION;
            static constexpr float P2 = 1.0f - END_TENSION * (1.0f - INFLEXION);

            static constexpr int NB_SAMPLES = 100;
            static float SPLINE_POSITION[NB_SAMPLES + 1];
            static float SPLINE_TIME[NB_SAMPLES + 1];

            /*
         * Get a signed deceleration that will reduce the velocity.
         */
            static float getDeceleration(int velocity);
            /*
         * Modifies mDuration to the duration it takes to get from start to newFinal using the
         * spline interpolation. The previous duration was needed to get to oldFinal.
         */
            void adjustDuration(int start, int oldFinal, int newFinal);

            double getSplineDeceleration(int velocity);

            double getSplineFlingDistance(int velocity);

            /* Returns the duration, expressed in milliseconds */
            int getSplineFlingDuration(int velocity);

            void fitOnBounceCurve(int start, int end, int velocity);

            void startSpringback(int start, int end, int velocity);

            void startBounceAfterEdge(int start, int end, int velocity);
            void startAfterEdge(int start, int min, int max, int velocity);
            void onEdgeReached();
        public:
            static void init();

            SplineOverScroller(Context& context);

            void setFriction(float friction);

            void updateScroll(float q);

            void startScroll(int start, int distance, int duration);

            void finish();

            void setFinalPosition(int position);

            void extendDuration(int extend);

            bool springback(int start, int min, int max);

            void fling(int start, int velocity, int min, int max, int over);

            void notifyEdgeReached(int start, int end, int over);

            bool continueWhenFinished();

            /*
            * Update the current position and velocity for current time. Returns
            * true if update has been done and false if animation duration has been
            * reached.
            */
            bool update();
        };

        SplineOverScroller *mScrollerX;
        SplineOverScroller *mScrollerY;
    };
}

#endif //BGFX_STUDY_OVERSCROLLER_H
