//
// Created by Administrator on 2020/12/28 0028.
//

#ifndef BGFX_STUDY_OVERSCROLLER_H
#define BGFX_STUDY_OVERSCROLLER_H

#include <application/Context.h>
#include "../Interpolation.h"
#include "../../application/ViewConfiguration.h"

namespace h7 {

    // from android
    class OverScroller {

    private:
        unsigned char mMode;

        Interpolation *mInterpolator;

        bool mFlywheel;

        static constexpr int DEFAULT_DURATION = 250;
        static constexpr unsigned char SCROLL_MODE = 0;
        static constexpr unsigned char FLING_MODE = 1;

        class SplineOverScroller {
        private:
            friend class OverScroller;

            static constexpr int SPLINE = 0;
            static constexpr int CUBIC = 1;
            static constexpr int BALLISTIC = 2;

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

            SplineOverScroller(Context &context);

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

    public:
        /**
         * Creates an OverScroller.
         * @param context The context of this application.
         * @param interpolator The scroll interpolator. If null, a default (viscous) interpolator will
         * be used.
         * @param flywheel If true, successive fling motions will keep on increasing scroll speed.
         * @hide
         */
        OverScroller(Context &context, Interpolation *inter, bool flywheel);

        OverScroller(Context &context, Interpolation *inter);

        OverScroller(Context &context);
        ~OverScroller();

        /**
        * The amount of friction applied to flings. The default value
        * is {@link ViewConfiguration#getScrollFriction}.
        *
        * @param friction A scalar dimension-less value representing the coefficient of
        *         friction.
        */
        void setFriction(float friction);

        /**
    *
    * Returns whether the scroller has finished scrolling.
    *
    * @return True if the scroller has finished scrolling, false otherwise.
    */
        bool isFinished();

        /**
      * Force the finished field to a particular value. Contrary to
      * {@link #abortAnimation()}, forcing the animation to finished
      * does NOT cause the scroller to move to the final x and y
      * position.
      *
      * @param finished The new finished value.
      */
        void forceFinished(bool finished);

        /**
     * Returns the current X offset in the scroll.
     *
     * @return The new X offset as an absolute distance from the origin.
     */
        int getCurrX();

        /**
        * Returns the current Y offset in the scroll.
        *
        * @return The new Y offset as an absolute distance from the origin.
        */
        int getCurrY();

        /**
         * Returns the absolute value of the current velocity.
         *
         * @return The original velocity less the deceleration, norm of the X and Y velocity vector.
         */
        float getCurrVelocity();

        /**
        * Returns the start X offset in the scroll.
        *
        * @return The start X offset as an absolute distance from the origin.
        */
        int getStartX();

        /**
        * Returns the start Y offset in the scroll.
        *
        * @return The start Y offset as an absolute distance from the origin.
        */
        int getStartY();

        /**
   * Returns where the scroll will end. Valid only for "fling" scrolls.
   *
   * @return The final X offset as an absolute distance from the origin.
   */
        int getFinalX();

        /**
        * Returns where the scroll will end. Valid only for "fling" scrolls.
        *
        * @return The final Y offset as an absolute distance from the origin.
        */
        int getFinalY();

        /**
       * Returns how long the scroll event will take, in milliseconds.
       *
       * @return The duration of the scroll in milliseconds.
       *
       * @hide Pending removal once nothing depends on it
       * @deprecated OverScrollers don't necessarily have a fixed duration.
       *             This function will lie to the best of its ability.
       */
        int getDuration();

        /**
        * Extend the scroll animation. This allows a running animation to scroll
        * further and longer, when used with {@link #setFinalX(int)} or {@link #setFinalY(int)}.
        *
        * @param extend Additional time to scroll in milliseconds.
        * @see #setFinalX(int)
        * @see #setFinalY(int)
        *
        * @hide Pending removal once nothing depends on it
        * @deprecated OverScrollers don't necessarily have a fixed duration.
        *             Instead of setting a new final position and extending
        *             the duration of an existing scroll, use startScroll
        *             to begin a new animation.
        */
        void extendDuration(int extend);

        /**
       * Sets the final position (X) for this scroller.
       *
       * @param newX The new X offset as an absolute distance from the origin.
       * @see #extendDuration(int)
       * @see #setFinalY(int)
       *
       * @hide Pending removal once nothing depends on it
       * @deprecated OverScroller's final position may change during an animation.
       *             Instead of setting a new final position and extending
       *             the duration of an existing scroll, use startScroll
       *             to begin a new animation.
       */
        void setFinalX(int newX);

        /**
       * Sets the final position (Y) for this scroller.
       *
       * @param newY The new Y offset as an absolute distance from the origin.
       * @see #extendDuration(int)
       * @see #setFinalX(int)
       *
       * @hide Pending removal once nothing depends on it
       * @deprecated OverScroller's final position may change during an animation.
       *             Instead of setting a new final position and extending
       *             the duration of an existing scroll, use startScroll
       *             to begin a new animation.
       */
        void setFinalY(int newY);

        /**
      * Call this when you want to know the new location. If it returns true, the
      * animation is not yet finished.
      */
        bool computeScrollOffset();

        /**
    * Start scrolling by providing a starting point and the distance to travel.
    * The scroll will use the default value of 250 milliseconds for the
    * duration.
    *
    * @param startX Starting horizontal scroll offset in pixels. Positive
    *        numbers will scroll the content to the left.
    * @param startY Starting vertical scroll offset in pixels. Positive numbers
    *        will scroll the content up.
    * @param dx Horizontal distance to travel. Positive numbers will scroll the
    *        content to the left.
    * @param dy Vertical distance to travel. Positive numbers will scroll the
    *        content up.
    */
        void startScroll(int startX, int startY, int dx, int dy);

        /**
        * Start scrolling by providing a starting point and the distance to travel.
        *
        * @param startX Starting horizontal scroll offset in pixels. Positive
        *        numbers will scroll the content to the left.
        * @param startY Starting vertical scroll offset in pixels. Positive numbers
        *        will scroll the content up.
        * @param dx Horizontal distance to travel. Positive numbers will scroll the
        *        content to the left.
        * @param dy Vertical distance to travel. Positive numbers will scroll the
        *        content up.
        * @param duration Duration of the scroll in milliseconds.
        */
        void startScroll(int startX, int startY, int dx, int dy, int duration);

        /**
      * Call this when you want to 'spring back' into a valid coordinate range.
      *
      * @param startX Starting X coordinate
      * @param startY Starting Y coordinate
      * @param minX Minimum valid X value
      * @param maxX Maximum valid X value
      * @param minY Minimum valid Y value
      * @param maxY Minimum valid Y value
      * @return true if a springback was initiated, false if startX and startY were
      *          already within the valid range.
      */
        bool springBack(int startX, int startY, int minX, int maxX, int minY, int maxY);

        void fling(int startX, int startY, int velocityX, int velocityY,
                   int minX, int maxX, int minY, int maxY);

        /**
      * Start scrolling based on a fling gesture. The distance traveled will
      * depend on the initial velocity of the fling.
      *
      * @param startX Starting point of the scroll (X)
      * @param startY Starting point of the scroll (Y)
      * @param velocityX Initial velocity of the fling (X) measured in pixels per
      *            second.
      * @param velocityY Initial velocity of the fling (Y) measured in pixels per
      *            second
      * @param minX Minimum X value. The scroller will not scroll past this point
      *            unless overX > 0. If overfling is allowed, it will use minX as
      *            a springback boundary.
      * @param maxX Maximum X value. The scroller will not scroll past this point
      *            unless overX > 0. If overfling is allowed, it will use maxX as
      *            a springback boundary.
      * @param minY Minimum Y value. The scroller will not scroll past this point
      *            unless overY > 0. If overfling is allowed, it will use minY as
      *            a springback boundary.
      * @param maxY Maximum Y value. The scroller will not scroll past this point
      *            unless overY > 0. If overfling is allowed, it will use maxY as
      *            a springback boundary.
      * @param overX Overfling range. If > 0, horizontal overfling in either
      *            direction will be possible.
      * @param overY Overfling range. If > 0, vertical overfling in either
      *            direction will be possible.
      */
        void fling(int startX, int startY, int velocityX, int velocityY,
                   int minX, int maxX, int minY, int maxY, int overX, int overY);

        /**
      * Notify the scroller that we've reached a horizontal boundary.
      * Normally the information to handle this will already be known
      * when the animation is started, such as in a call to one of the
      * fling functions. However there are cases where this cannot be known
      * in advance. This function will transition the current motion and
      * animate from startX to finalX as appropriate.
      *
      * @param startX Starting/current X position
      * @param finalX Desired final X position
      * @param overX Magnitude of overscroll allowed. This should be the maximum
      *              desired distance from finalX. Absolute value - must be positive.
      */
        void notifyHorizontalEdgeReached(int startX, int finalX, int overX);

        /**
       * Notify the scroller that we've reached a vertical boundary.
       * Normally the information to handle this will already be known
       * when the animation is started, such as in a call to one of the
       * fling functions. However there are cases where this cannot be known
       * in advance. This function will animate a parabolic motion from
       * startY to finalY.
       *
       * @param startY Starting/current Y position
       * @param finalY Desired final Y position
       * @param overY Magnitude of overscroll allowed. This should be the maximum
       *              desired distance from finalY. Absolute value - must be positive.
       */
        void notifyVerticalEdgeReached(int startY, int finalY, int overY);

        /**
      * Returns whether the current Scroller is currently returning to a valid position.
      * Valid bounds were provided by the
      * {@link #fling(int, int, int, int, int, int, int, int, int, int)} method.
      *
      * One should check this value before calling
      * {@link #startScroll(int, int, int, int)} as the interpolation currently in progress
      * to restore a valid position will then be stopped. The caller has to take into account
      * the fact that the started scroll will start from an overscrolled position.
      *
      * @return true when the current position is overscrolled and in the process of
      *         interpolating back to a valid value.
      */
        bool isOverScrolled();

        /**
      * Stops the animation. Contrary to {@link #forceFinished(boolean)},
      * aborting the animating causes the scroller to move to the final x and y
      * positions.
      *
      * @see #forceFinished(boolean)
      */
        void abortAnimation();

        /**
      * Returns the time elapsed since the beginning of the scrolling.
      *
      * @return The elapsed time in milliseconds.
      *
      * @hide
      */
        int timePassed();

        /**
        * @hide
        */
        bool isScrollingInDirection(float xvel, float yvel);
    };
}

#endif //BGFX_STUDY_OVERSCROLLER_H
