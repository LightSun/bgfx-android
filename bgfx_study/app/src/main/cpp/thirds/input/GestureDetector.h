//
// Created by Administrator on 2020/11/22 0022.
//

#ifndef BGFX_STUDY_GESTUREDETECTOR_H
#define BGFX_STUDY_GESTUREDETECTOR_H

#include <math.h>
#include "Task.h"
#include "InputProcessor.h"
#include "VelocityTracker.h"

namespace h7 {
    class GestureDetector : public InputProcessor {

    private:
        GestureListener* listener;
        float tapRectangleWidth;
        float tapRectangleHeight;
        uint64_t tapCountInterval;
        float longPressSeconds;
        long maxFlingDelay;

        bool inTapRectangle;
        int tapCount;
        uint64_t lastTapTime;
        float lastTapX, lastTapY;
        int lastTapButton, lastTapPointer;
        bool longPressFired;
        bool pinching;
        bool panning;

        VelocityTracker tracker;
        float tapRectangleCenterX, tapRectangleCenterY;
        uint64_t touchDownTime;
        Vector2f pointer1;
        Vector2f pointer2;
        Vector2f initialPointer1;
        Vector2f initialPointer2;

        SchedulerTask longPressTask;
    public:
        /** Creates a new GestureDetector with default values: halfTapSquareSize=20, tapCountInterval=0.4f, longPressDuration=1.1f,
	 * maxFlingDelay=Integer.MAX_VALUE. */
        GestureDetector(GestureListener* listener);

        /** @param halfTapSquareSize half width in pixels of the square around an initial touch event, see
	 *           {@link GestureListener#tap(float, float, int, int)}.
	 * @param tapCountInterval time in seconds that must pass for two touch down/up sequences to be detected as consecutive taps.
	 * @param longPressDuration time in seconds that must pass for the detector to fire a
	 *           {@link GestureListener#longPress(float, float)} event.
	 * @param maxFlingDelay no fling event is fired when the time in seconds the finger was dragged is larger than this, 	see
	 *           {@link GestureListener#fling(float, float, int)} */
        GestureDetector(float halfTapSquareSize, float tapCountInterval, float longPressDuration,
                        float maxFlingDelay,
                        GestureListener* listener);

        /**
         * @param halfTapRectangleWidth half width in pixels of the rectangle around an initial touch event, see
        *           {@link GestureListener#tap(float, float, int, int)}.
        * @param halfTapRectangleHeight half height in pixels of the rectangle around an initial touch event, see
        *           {@link GestureListener#tap(float, float, int, int)}.
        * @param tapCountInterval time in seconds that must pass for two touch down/up sequences to be detected as consecutive taps.
        * @param longPressDuration time in seconds that must pass for the detector to fire a
        *           {@link GestureListener#longPress(float, float)} event.
        * @param maxFlingDelay no fling event is fired when the time in seconds the finger was dragged is larger than this, see
        *           {@link GestureListener#fling(float, float, int)} */
        GestureDetector(float halfTapRectangleWidth, float halfTapRectangleHeight,
                        float tapCountInterval,
                        float longPressDuration, float maxFlingDelay, GestureListener* listener);

        bool touchDown (int x, int y, int pointer, int button) ;
        bool touchDown (float x, float y, int pointer, int button) ;

        bool touchDragged (int x, int y, int pointer);
        bool touchDragged (float x, float y, int pointer);
        bool touchUp (int x, int y, int pointer, int button);
        bool touchUp (float x, float y, int pointer, int button);
        /** No further gesture events will be triggered for the current touch, if any. */
        void cancel ();
        bool isLongPressed ();
        bool isLongPressed (float duration);
        bool isPanning ();
        void reset ();

        /** The tap square will no longer be used for the current touch. */
        void invalidateTapSquare ();
        void setTapSquareSize (float halfTapSquareSize);
        void setTapRectangleSize (float halfTapRectangleWidth, float halfTapRectangleHeight);
        /** @param tapCountInterval time in seconds that must pass for two touch down/up sequences to be detected as consecutive
	 *           taps. */
        void setTapCountInterval (float tapCountInterval);
        void setLongPressSeconds (float longPressSeconds);
        void setMaxFlingDelay (long maxFlingDelay);
    private:
        inline bool isWithinTapRectangle (float x, float y, float centerX, float centerY) {
            return ::fabsf(x - centerX) < tapRectangleWidth && ::fabsf(y - centerY) < tapRectangleHeight;
        }
    };
}

#endif //BGFX_STUDY_GESTUREDETECTOR_H
