//
// Created by Administrator on 2020/11/22 0022.
//

#include "GestureDetector.h"
#include <limits.h>
#include "GestureContext.h"
#include "utils/TimeUtils.hpp"

namespace h7 {
    GestureDetector::GestureDetector(h7::GestureListener *listener) : GestureDetector(20, 0.4f,
                                                                                      1.1f, INT_MAX,
                                                                                      listener) {

    }

    GestureDetector::GestureDetector(float halfTapSquareSize, float tapCountInterval,
                                     float longPressDuration, float maxFlingDelay,
                                     h7::GestureListener *listener) :
            GestureDetector(halfTapSquareSize, halfTapSquareSize, tapCountInterval,
                            longPressDuration, maxFlingDelay, listener) {

    }

    GestureDetector::GestureDetector(float halfTapRectangleWidth, float halfTapRectangleHeight,
                                     float tapCountInterval, float longPressDuration,
                                     float maxFlingDelay, h7::GestureListener *listener) {
        //static_assert(listener != nullptr, "listener cannot be null.");
        this->tapRectangleWidth = halfTapRectangleWidth;
        this->tapRectangleHeight = halfTapRectangleHeight;
        this->tapCountInterval = (uint64_t) (tapCountInterval * 1000000000l);
        this->longPressSeconds = longPressDuration;
        this->maxFlingDelay = (uint64_t) (maxFlingDelay * 1000000000l);
        this->listener = listener;

        auto task = [&]() {
            if (!longPressFired) longPressFired = listener->longPress(pointer1.x, pointer1.y);
            return NULL;
        };
        longPressTask.set(task);
    }

    bool GestureDetector::touchDown(int x, int y, int pointer, int button) {
        return touchDown((float) x, (float) y, pointer, button);
    }

    bool GestureDetector::touchDown(float x, float y, int pointer, int button) {
        if (pointer > 1) return false;

        if (pointer == 0) {
            pointer1.set(x, y);
            touchDownTime = getCurrentEventTime();
            tracker.start(x, y, touchDownTime);
            if (isTouched(1)) {
                // Start pinch.
                inTapRectangle = false;
                pinching = true;
                initialPointer1.set(pointer1);
                initialPointer2.set(pointer2);
                longPressTask.cancel();
            } else {
                // Normal touch down.
                inTapRectangle = true;
                pinching = false;
                longPressFired = false;
                tapRectangleCenterX = x;
                tapRectangleCenterY = y;
                // if (!longPressTask.isScheduled()) Timer.schedule(longPressTask, longPressSeconds);
                if (!longPressTask.isScheduled()) longPressTask.schedule(longPressSeconds);
            }
        } else {
            // Start pinch.
            pointer2.set(x, y);
            inTapRectangle = false;
            pinching = true;
            initialPointer1.set(pointer1);
            initialPointer2.set(pointer2);
            longPressTask.cancel();
        }
        return listener->touchDown(x, y, pointer, button);
    }

    bool GestureDetector::touchDragged(int x, int y, int pointer) {
        return touchDragged((float) x, (float) y, pointer);
    }

    bool GestureDetector::touchDragged(float x, float y, int pointer) {
        if (pointer > 1) return false;
        if (longPressFired) return false;

        if (pointer == 0)
            pointer1.set(x, y);
        else
            pointer2.set(x, y);

        // handle pinch zoom
        if (pinching) {
            if (listener != NULL) {
                bool result = listener->pinch(initialPointer1, initialPointer2, pointer1, pointer2);
                return listener->zoom(initialPointer1.dst(initialPointer2),
                                      pointer1.dst(pointer2)) || result;
            }
            return false;
        }

        // update tracker
        tracker.update(x, y, getCurrentEventTime());

        // check if we are still tapping.
        if (inTapRectangle &&
            !isWithinTapRectangle(x, y, tapRectangleCenterX, tapRectangleCenterY)) {
            longPressTask.cancel();
            inTapRectangle = false;
        }

        // if we have left the tap square, we are panning
        if (!inTapRectangle) {
            panning = true;
            return listener->pan(x, y, tracker.deltaX, tracker.deltaY);
        }

        return false;
    }

    bool GestureDetector::touchUp(int x, int y, int pointer, int button) {
        return touchUp((float) x, (float) y, pointer, button);
    }

    bool GestureDetector::touchUp(float x, float y, int pointer, int button) {
        if (pointer > 1) return false;

        // check if we are still tapping.
        if (inTapRectangle &&
            !isWithinTapRectangle(x, y, tapRectangleCenterX, tapRectangleCenterY))
            inTapRectangle = false;

        bool wasPanning = panning;
        panning = false;

        longPressTask.cancel();
        if (longPressFired) return false;

        if (inTapRectangle) {
            // handle taps
            if (lastTapButton != button || lastTapPointer != pointer ||
                nanoTime() - lastTapTime > tapCountInterval
                || !isWithinTapRectangle(x, y, lastTapX, lastTapY))
                tapCount = 0;
            tapCount++;
            lastTapTime = nanoTime();
            lastTapX = x;
            lastTapY = y;
            lastTapButton = button;
            lastTapPointer = pointer;
            touchDownTime = 0;
            return listener->tap(x, y, tapCount, button);
        }

        if (pinching) {
            // handle pinch end
            pinching = false;
            listener->pinchStop();
            panning = true;
            // we are in pan mode again, reset velocity tracker
            if (pointer == 0) {
                // first pointer has lifted off, set up panning to use the second pointer...
                tracker.start(pointer2.x, pointer2.y, getCurrentEventTime());
            } else {
                // second pointer has lifted off, set up panning to use the first pointer...
                tracker.start(pointer1.x, pointer1.y, getCurrentEventTime());
            }
            return false;
        }

        // handle no longer panning
        bool handled = false;
        if (wasPanning && !panning) handled = listener->panStop(x, y, pointer, button);

        // handle fling
        unsigned long long time = getCurrentEventTime();
        if (time - touchDownTime <= maxFlingDelay) {
            tracker.update(x, y, time);
            handled = listener->fling(tracker.getVelocityX(), tracker.getVelocityY(), button) ||
                      handled;
        }
        touchDownTime = 0;
        return handled;
    }

    void GestureDetector::cancel() {
        longPressTask.cancel();
        longPressFired = true;
    }

    bool GestureDetector::isLongPressed() {
        return isLongPressed(longPressSeconds);
    }

    bool GestureDetector::isLongPressed(float duration) {
        if (touchDownTime == 0) return false;
        return nanoTime() - touchDownTime > (uint64_t) (duration * 1000000000l);
    }

    bool GestureDetector::isPanning() {
        return panning;
    }

    void GestureDetector::reset() {
        touchDownTime = 0;
        panning = false;
        inTapRectangle = false;
        tracker.lastTime = 0;
    }

    void GestureDetector::invalidateTapSquare() {
        inTapRectangle = false;
    }

    void GestureDetector::setTapSquareSize(float halfTapSquareSize) {
        setTapRectangleSize(halfTapSquareSize, halfTapSquareSize);
    }

    void GestureDetector::setTapRectangleSize(float halfTapRectangleWidth,
                                              float halfTapRectangleHeight) {
        this->tapRectangleWidth = halfTapRectangleWidth;
        this->tapRectangleHeight = halfTapRectangleHeight;
    }

    /** @param tapCountInterval time in seconds that must pass for two touch down/up sequences to be detected as consecutive
     *           taps. */
    void GestureDetector::setTapCountInterval(float tapCountInterval) {
        this->tapCountInterval = (uint64_t) (tapCountInterval * 1000000000l);
    }
    void GestureDetector::setLongPressSeconds(float longPressSeconds) {
        this->longPressSeconds = longPressSeconds;
    }
    void GestureDetector::setMaxFlingDelay(long maxFlingDelay) {
        this->maxFlingDelay = maxFlingDelay;
    }
}