//
// Created by Administrator on 2020/11/24 0024.
//

#include <stdlib.h>
#include "AndroidInput.h"
#include "../../../core/log.h"
#include <luaext_java/java_env.h>

#define AND_INPUT_CLASS  "com/heaven7/android/hbmdx/input/AndroidInput"
#define AND_INPUT_SIG "L" AND_INPUT_CLASS ";"
#define SIG_KEYBOARD "(" AND_INPUT_SIG "ZI)V"

static jclass _andInputClazz;

namespace h7{
    void AndroidInput::release() {
        if(pressure) free(pressure);
        if(realId) free(realId);
        if(touchX) free(touchX);
        if(touchY) free(touchY);
        if(deltaX) free(deltaX);
        if(deltaY) free(deltaY);
        if(touched) free(touched);
        if(button) free(button);
    }
    void AndroidInput::ensureSize(int expect) {
        int oldLen = len_realId;
        int* realId = (int*)malloc(sizeof(int) * expect);
        int* touchX = (int*)malloc(sizeof(int) * expect);
        int* touchY = (int*)malloc(sizeof(int) * expect);
        int* deltaX = (int*)malloc(sizeof(int) * expect);
        int* deltaY = (int*)malloc(sizeof(int) * expect);
        int* button = (int*)malloc(sizeof(int) * expect);

        float* pressure = (float*)malloc(sizeof(float) * expect);
        bool* touched = (bool*)malloc(sizeof(bool) * expect);
#define RESET_FIELD(x, unitType)\
if(this->x){\
    memcpy(x, this->x, oldLen * sizeof(unitType));\
    free(this->x);\
    this->x = x;\
}
        //copy value and release old
        RESET_FIELD(realId, int)
        RESET_FIELD(touchX, int)
        RESET_FIELD(touchY, int)
        RESET_FIELD(deltaX, int)
        RESET_FIELD(deltaY, int)
        RESET_FIELD(button, int)
        RESET_FIELD(pressure, float)
        RESET_FIELD(touched, bool)

        len_realId = expect;
    }
    AndroidInput::AndroidInput() {
        if(_andInputClazz == NULL){
            _andInputClazz = getJNIEnv()->FindClass(AND_INPUT_CLASS);
        }

        ensureSize(NUM_TOUCHES);
        for (int i = 0; i < len_realId; i++){
            realId[i] = -1;
        }
        // this is for backward compatibility: libGDX always caught the circle button, original comment:
        // circle button on Xperia Play shouldn't need catchBack == true
        keysToCatch.add(Keys::BUTTON_CIRCLE);
    }
    AndroidInput::~AndroidInput() {
        release();
    }
    int AndroidInput::getFreePointerIndex() {
        int len = len_realId;
        for (int i = 0; i < len; i++) {
            if (realId[i] == -1) return i;
        }
        ensureSize(len + 2);
        /*pressure = resize(pressure);
        realId = resize(realId);
        touchX = resize(touchX);
        touchY = resize(touchY);
        deltaX = resize(deltaX);
        deltaY = resize(deltaY);
        touched = resize(touched);
        button = resize(button);*/

        return len;
    }
    int AndroidInput::lookUpPointerIndex(int pointerId) {
        for (int i = 0; i < NUM_TOUCHES; i++) {
            if (realId[i] == pointerId) return i;
        }
        LOGW("lookUpPointerIndex: can't find pointer index for pointerId = %d", pointerId);
        return -1;
    }
    bool AndroidInput::isPeripheralAvailable(h7::Peripheral peripheral) {
        switch (peripheral){
            case Accelerometer:
                return accelerometerAvailable;
            case Gyroscope:
                return gyroscopeAvailable;
            case Compass:
                return compassAvailable;
            case HardwareKeyboard:
                return keyboardAvailable;
            case OnscreenKeyboard:
                return true;
            case Vibrator:
                //TODO  return vibrator != null && vibrator.hasVibrator();
                return true;
            case MultitouchScreen:
                return hasMultitouch;
            case RotationVector:
                return rotationVectorAvailable;
            case Pressure:
                return true;
        }
        return false;
    }
    bool AndroidInput::isKeyPressed(int key) {
        if (key == Input::Keys::ANY_KEY) {
            return keyCount > 0;
        }
        if (key < 0 || key >= SUPPORTED_KEYS) {
            return false;
        }
        return keys[key];
    }
    bool AndroidInput::isKeyJustPressed(int key) {
        if (key == Input::Keys::ANY_KEY) {
            return keyJustPressed;
        }
        if (key < 0 || key >= SUPPORTED_KEYS) {
            return false;
        }
        return justPressedKeys[key];
    }
    void AndroidInput::setCatchKey(int keycode, bool catchKey) {
        if (!catchKey) {
            keysToCatch.remove(keycode);
        } else if (catchKey) {
            keysToCatch.add(keycode);
        }
    }
    bool AndroidInput::isCatchKey(int keycode) {
        return keysToCatch.contains(keycode);
    }

    bool AndroidInput::isTouched() {
        lockTouch();
        if (hasMultitouch) {
            for (int pointer = 0; pointer < NUM_TOUCHES; pointer++) {
                if (touched[pointer]) {
                    unlockTouch();
                    return true;
                }
            }
        }
        auto r = touched[0];
        unlockTouch();
        return r;
    }
    void AndroidInput::processEvents() {
        lockTouch();
        if (_justTouched) {
            _justTouched = false;
            for (int i = 0; i < NUM_PRESS_BUTTON; i++) {
                justPressedButtons[i] = false;
            }
        }
        if (keyJustPressed) {
            keyJustPressed = false;
            for (int i = 0; i < SUPPORTED_KEYS; i++) {
                justPressedKeys[i] = false;
            }
        }

        if (_inputProcessor != NULL) {
            InputProcessor& processor = *_inputProcessor;

            int len = keyEvents.size();
            for (int i = 0; i < len; i++) {
                KeyEvent* e = keyEvents.get(i);
                currentEventTimeStamp = e->timeStamp;
                switch (e->type) {
                    case KeyEvent::KEY_DOWN:
                        processor.keyDown(e->keyCode);
                        keyJustPressed = true;
                        justPressedKeys[e->keyCode] = true;
                        break;
                    case KeyEvent::KEY_UP:
                        processor.keyUp(e->keyCode);
                        break;
                    case KeyEvent::KEY_TYPED:
                        processor.keyTyped(e->keyChar);
                }
                usedKeyEvents.free(e);
            }

            len = touchEvents.size();
            for (int i = 0; i < len; i++) {
                TouchEvent& e = *touchEvents.get(i);
                currentEventTimeStamp = e.timeStamp;
                switch (e.type) {
                    case TouchEvent::TOUCH_DOWN:
                        processor.touchDown(e.x, e.y, e.pointer, e.button);
                        _justTouched = true;
                        justPressedButtons[e.button] = true;
                        break;
                    case TouchEvent::TOUCH_UP:
                        processor.touchUp(e.x, e.y, e.pointer, e.button);
                        break;
                    case TouchEvent::TOUCH_DRAGGED:
                        processor.touchDragged(e.x, e.y, e.pointer);
                        break;
                    case TouchEvent::TOUCH_MOVED:
                        processor.mouseMoved(e.x, e.y);
                        break;
                    case TouchEvent::TOUCH_SCROLLED:
                        processor.scrolled(e.scrollAmountX, e.scrollAmountY);
                }
                usedTouchEvents.free(&e);
            }
        } else {
            int len = touchEvents.size();
            for (int i = 0; i < len; i++) {
                TouchEvent& e = *touchEvents.get(i);
                if (e.type == TouchEvent::TOUCH_DOWN) _justTouched = true;
                usedTouchEvents.free(&e);
            }

            len = keyEvents.size();
            for (int i = 0; i < len; i++) {
                usedKeyEvents.free(keyEvents.get(i));
            }
        }

        if (touchEvents.isEmpty()) {
            for (int i = 0; i < len_realId; i++) {
                deltaX[i] = 0;
                deltaY[i] = 0;
            }
        }
        keyEvents.clear();
        touchEvents.clear();
        unlockTouch();
    }
    bool AndroidInput::isButtonPressed(int _button) {
        lockTouch();
        if (hasMultitouch) {
            for (int pointer = 0; pointer < NUM_TOUCHES; pointer++) {
                if (touched[pointer] && (button[pointer] == _button)) {
                    unlockTouch();
                    return true;
                }
            }
        }
        auto result = (touched[0] && (button[0] == _button));
        unlockTouch();
        return result;
    }
    float AndroidInput::getAzimuth () {
        if (!compassAvailable && !rotationVectorAvailable) return 0;
        updateOrientation();
        return azimuth;
    }
    float AndroidInput::getPitch () {
        if (!compassAvailable && !rotationVectorAvailable) return 0;
        updateOrientation();
        return pitch;
    }
    float AndroidInput::getRoll () {
        if (!compassAvailable && !rotationVectorAvailable) return 0;

        updateOrientation();
        return roll;
    }
    void AndroidInput::setOnscreenKeyboardVisible(bool visible, h7::OnscreenKeyboardType type) {
       //TODO  setOnscreenKeyboardVisible
        auto pEnv = getJNIEnv();
        if(pEnv == NULL){
            pEnv = attachJNIEnv();
        }
        auto mid = pEnv->GetStaticMethodID(_andInputClazz, "setOnscreenKeyboardVisible",
                                           SIG_KEYBOARD);
        //pEnv->CallStaticVoidMethod(_andInputClazz, mid, )
    }
}