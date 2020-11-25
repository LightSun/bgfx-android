//
// Created by Administrator on 2020/11/24 0024.
//

#include "AndroidTouchHandler.h"
#include "AndroidInput.h"
#include "AndroidInputJni.h"
#include <input/GestureContext.h>

namespace h7 {
    static int toGdxButton(int button) {
        if (button == 0 || button == 1) return Buttons::LEFT;
        if (button == 2) return Buttons::RIGHT;
        if (button == 4) return Buttons::MIDDLE;
        if (button == 8) return Buttons::BACK;
        if (button == 16) return Buttons::FORWARD;
        return -1;
    }

    static void postTouchEvent(AndroidInput *input, int type, int x, int y, int pointer, int button,
                               long long timeStamp) {
        TouchEvent *event = input->usedTouchEvents.obtain();
        event->timeStamp = timeStamp;
        event->pointer = pointer;
        event->x = x;
        event->y = y;
        event->type = type;
        event->button = button;
        input->touchEvents.add(event);
    }

    void android_onTouch(AndroidInput *ainput, MotionEventWrapper *event) {
        AndroidInput &input = *ainput;

        int action = event->action;
        int pointerIndex = event->pointerIndex;
        int pointerId = event->pointerId;

        int x = 0;
        int y = 0;
        int realPointerIndex = 0;
        int button = Buttons::LEFT;

        long long timeStamp = event->timeStamp;
        ainput->lockTouch();
        switch (action) {
            case MotionEventWrapper::ACTION_DOWN:
            case MotionEventWrapper::ACTION_POINTER_DOWN:
                realPointerIndex = input.getFreePointerIndex(); // get a free pointer index as reported by Input.getX() etc.
                if (realPointerIndex >= AndroidInput::NUM_TOUCHES) break;
                input.realId[realPointerIndex] = pointerId;
                x = event->x;
                y = event->y;
                button = toGdxButton(event->buttonState);
                if (button != -1)
                    postTouchEvent(ainput, TouchEvent::TOUCH_DOWN, x, y, realPointerIndex, button,
                                   timeStamp);
                input.touchX[realPointerIndex] = x;
                input.touchY[realPointerIndex] = y;
                input.deltaX[realPointerIndex] = 0;
                input.deltaY[realPointerIndex] = 0;
                input.touched[realPointerIndex] = (button != -1);
                input.button[realPointerIndex] = button;
                input.pressure[realPointerIndex] = event->pressure;
                break;

            case MotionEventWrapper::ACTION_UP:
            case MotionEventWrapper::ACTION_POINTER_UP:
            case MotionEventWrapper::ACTION_OUTSIDE:
                realPointerIndex = input.lookUpPointerIndex(pointerId);
                if (realPointerIndex == -1) break;
                if (realPointerIndex >= AndroidInput::NUM_TOUCHES) break;
                input.realId[realPointerIndex] = -1;
                /*  x = (int)event.getX(pointerIndex);
                  y = (int)event.getY(pointerIndex);*/
                x = event->x;
                y = event->y;

                button = input.button[realPointerIndex];
                if (button != -1)
                    postTouchEvent(ainput, TouchEvent::TOUCH_UP, x, y, realPointerIndex, button,
                                   timeStamp);
                input.touchX[realPointerIndex] = x;
                input.touchY[realPointerIndex] = y;
                input.deltaX[realPointerIndex] = 0;
                input.deltaY[realPointerIndex] = 0;
                input.touched[realPointerIndex] = false;
                input.button[realPointerIndex] = 0;
                input.pressure[realPointerIndex] = 0;
                break;

            case MotionEventWrapper::ACTION_CANCEL:
                for (int i = 0; i < input.len_realId; i++) {
                    input.realId[i] = -1;
                    input.touchX[i] = 0;
                    input.touchY[i] = 0;
                    input.deltaX[i] = 0;
                    input.deltaY[i] = 0;
                    input.touched[i] = false;
                    input.button[i] = 0;
                    input.pressure[i] = 0;
                }
                break;

            case MotionEventWrapper::ACTION_MOVE:
                int pointerCount = event->pointerCount;
                for (int i = 0; i < pointerCount; i++) {
                    // pointerIndex = i;
                    event->getEventInfo(i);
                    pointerId = event->getPointerId(pointerIndex);
                    x = (int) event->getX(pointerIndex);
                    y = (int) event->getY(pointerIndex);
                    realPointerIndex = input.lookUpPointerIndex(pointerId);
                    if (realPointerIndex == -1) continue;
                    if (realPointerIndex >= AndroidInput::NUM_TOUCHES) break;
                    button = input.button[realPointerIndex];
                    if (button != -1)
                        postTouchEvent(ainput, TouchEvent::TOUCH_DRAGGED, x, y, realPointerIndex,
                                       button, timeStamp);
                    else
                        postTouchEvent(ainput, TouchEvent::TOUCH_MOVED, x, y, realPointerIndex, 0,
                                       timeStamp);
                    input.deltaX[realPointerIndex] = x - input.touchX[realPointerIndex];
                    input.deltaY[realPointerIndex] = y - input.touchY[realPointerIndex];
                    input.touchX[realPointerIndex] = x;
                    input.touchY[realPointerIndex] = y;
                    input.pressure[realPointerIndex] = event->getPressure(pointerIndex);
                }
                break;
        }
        event->jRecycle();
        ainput->unlockTouch();
        requestRender();
        // Gdx.app.getGraphics().requestRendering();
    }
}