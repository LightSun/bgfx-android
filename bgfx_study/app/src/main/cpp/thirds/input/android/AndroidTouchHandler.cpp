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
                    pointerId = event->getPointerId();
                    x = (int) event->getX();
                    y = (int) event->getY();
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
                    input.pressure[realPointerIndex] = event->getPressure();
                }
                break;
        }
        event->jRecycle();
        ainput->unlockTouch();
        requestRender();
        // Gdx.app.getGraphics().requestRendering();
    }

    bool android_onKey(AndroidInput* ainput, int keyCode, KeyEventWrapper* ev){
        AndroidInput& input = *ainput;
        KeyEventWrapper& e = *ev;
        for (int i = 0, n = input.keyListeners.size(); i < n; i++){
            if (input.keyListeners.get(i)->onKey(keyCode, ev)) return true;
        }
        // If the key is held sufficiently long that it repeats, then the initial down is followed
        // additional key events with ACTION_DOWN and a non-zero value for getRepeatCount().
        // We are only interested in the first key down event here and must ignore all others
        if (e.action == KeyEvent::ACTION_DOWN && e.repeatCount > 0)
            return input.keysToCatch.contains(keyCode);

        input.lockTouch();
        KeyEvent* event = NULL;

        if (e.keyCode == KeyEvent::KEYCODE_UNKNOWN && e.action == KeyEvent::ACTION_MULTIPLE) {
            const char* chars = e.chars;
            for (int i = 0; i < strlen(chars); i++) {
                event = input.usedKeyEvents.obtain();
                event->timeStamp = e.timeStamp;
                event->keyCode = 0;
                event->keyChar = chars[i];
                event->type = KeyEvent::KEY_TYPED;
                input.keyEvents.add(event);
            }
            input.unlockTouch();
            return false;
        }

        char character = (char)e.unicodeChar;
        // Android doesn't report a unicode char for back space. hrm...
        if (keyCode == 67) character = '\b';
        if (e.keyCode < 0 || e.keyCode >= AndroidInput::SUPPORTED_KEYS) {
            input.unlockTouch();
            return false;
        }

        switch (e.action) {
            case KeyEvent::ACTION_DOWN:
                event = input.usedKeyEvents.obtain();
                event->timeStamp = e.timeStamp;
                event->keyChar = 0;
                event->keyCode = e.keyCode;
                event->type = KeyEvent::KEY_DOWN;

                // Xperia hack for circle key. gah...
                if (keyCode == KeyEvent::KEYCODE_BACK && e.altPressed) {
                    keyCode = Input::Keys::BUTTON_CIRCLE;
                    event->keyCode = keyCode;
                }

                input.keyEvents.add(event);
                if (!input.keys[event->keyCode]) {
                    input.keyCount++;
                    input.keys[event->keyCode] = true;
                }
                break;
            case KeyEvent::ACTION_UP:
                long long timeStamp = e.timeStamp;
                event = input.usedKeyEvents.obtain();
                event->timeStamp = timeStamp;
                event->keyChar = 0;
                event->keyCode = e.keyCode;
                event->type = KeyEvent::KEY_UP;
                // Xperia hack for circle key. gah...
                if (keyCode == KeyEvent::KEYCODE_BACK && e.altPressed) {
                    keyCode = Input::Keys::BUTTON_CIRCLE;
                    event->keyCode = keyCode;
                }
                input.keyEvents.add(event);

                event = input.usedKeyEvents.obtain();
                event->timeStamp = timeStamp;
                event->keyChar = character;
                event->keyCode = 0;
                event->type = KeyEvent::KEY_TYPED;
                input.keyEvents.add(event);

                if (keyCode == Input::Keys::BUTTON_CIRCLE) {
                    if (input.keys[Input::Keys::BUTTON_CIRCLE]) {
                        input.keyCount--;
                        input.keys[Input::Keys::BUTTON_CIRCLE] = false;
                    }
                } else {
                    if (input.keys[e.keyCode]) {
                        input.keyCount--;
                        input.keys[e.keyCode] = false;
                    }
                }
        }
        requestRender();
        input.unlockTouch();
        return input.keysToCatch.contains(keyCode);
    }
}