//
// Created by Administrator on 2020/11/24 0024.
//

#include <stdlib.h>
#include "AndroidInput.h"
#include "../../../core/log.h"
#include <luaext_java/java_env.h>
#include "bx/math.h"
#include "../GestureContext.h"

#define AND_INPUT_CLASS  "com/heaven7/android/hbmdx/input/AndroidInput"
#define AND_INPUT_SIG "L" AND_INPUT_CLASS ";"
#define SIG_KEYBOARD "(" AND_INPUT_SIG "ZI)V"
#define SIG_getRotationMatrixFromVector "([F[F)V"
#define SIG_getRotationMatrix "([F[F[F[F)Z"
#define SIG_getOrientation "([F[F)V"
#define SIG_getRotation "(" AND_INPUT_SIG ")I"

#define SIG_vibrate_INT "(I)V"
#define SIG_vibrate "([JI)V"
#define SIG_vibrateCancel "()V"

#define LEN_R 9
#define LEN_rotationVector 3

static jclass _andInputClazz;

namespace h7{
    bool AndroidInput::getRotationMatrix0(jfloatArray _R, float *outR) {
        auto pEnv = ensureJniEnv();
        if (rotationVectorAvailable){
            auto mid = pEnv->GetStaticMethodID(_andInputClazz, "getRotationMatrixFromVector",
                                               SIG_getRotationMatrixFromVector);
            auto _rotationVectorValues = pEnv->NewFloatArray(LEN_rotationVector);
            pEnv->CallStaticVoidMethod(_andInputClazz, mid, _R, _rotationVectorValues);
            pEnv->GetFloatArrayRegion(_R, 0, LEN_R, outR);
            pEnv->GetFloatArrayRegion(_rotationVectorValues, 0, LEN_rotationVector, rotationVectorValues);
            //SensorManager.getRotationMatrixFromVector(R, rotationVectorValues);
            return true;
        } else {
            //if (!SensorManager.getRotationMatrix(R, null, accelerometerValues, magneticFieldValues)) {
            auto mid = pEnv->GetStaticMethodID(_andInputClazz, "getRotationMatrix",SIG_getRotationMatrix);
            auto _accelerometerValues = pEnv->NewFloatArray(3);
            auto _magneticFieldValues = pEnv->NewFloatArray(3);
            auto _I = pEnv->NewFloatArray(0);

            auto result = pEnv->CallStaticBooleanMethod(_andInputClazz, mid, _R, _I, _accelerometerValues, _magneticFieldValues);
            pEnv->GetFloatArrayRegion(_R, 0, LEN_R, outR);
            pEnv->GetFloatArrayRegion(_accelerometerValues, 0, 3, accelerometerValues);
            pEnv->GetFloatArrayRegion(_magneticFieldValues, 0, 3, magneticFieldValues);
            //if !result:  // compass + accelerometer in free fall
            return result;
        }
    }
    void AndroidInput::getRotationMatrix(float *matrix) {
        auto pEnv = ensureJniEnv();
        auto _R = pEnv->NewFloatArray(LEN_R);
        getRotationMatrix0(_R, matrix);
    }
    void AndroidInput::updateOrientation() {
        auto pEnv = ensureJniEnv();
        auto _R = pEnv->NewFloatArray(LEN_R);
        //false means: compass + accelerometer in free fall
        if(getRotationMatrix0(_R, R)){
            auto mid = pEnv->GetStaticMethodID(_andInputClazz, "getOrientation", SIG_getOrientation);
            auto _orientation = pEnv->NewFloatArray(3);
            pEnv->CallStaticVoidMethod(_andInputClazz, mid, _R, _orientation);
            pEnv->GetFloatArrayRegion(_R, 0, LEN_R, R);
            pEnv->GetFloatArrayRegion(_orientation, 0, 3, orientation);
            //SensorManager.getOrientation(R, orientation);
            azimuth = bx::toDeg(orientation[0]);
            pitch = bx::toDeg(orientation[1]);
            roll = bx::toDeg(orientation[2]);
        }
    }
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
        keysToCatch.add(cCast_ref(int, Keys::BUTTON_CIRCLE));
    }
    AndroidInput::~AndroidInput(){
        release();
        lockTouch();
        usedKeyEvents.clear();
        usedTouchEvents.clear();
        unlockTouch();
        //listeners
        auto it_key = OnKeyListener::Iterator();
        keyListeners.clear(&it_key);
        //
        auto it_motion = OnGenericMotionListener::Iterator();
        genericMotionListeners.clear(&it_motion);
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
        //recycle event
        auto it_key = KeyEvent::Iterator();
        keyEvents.clear(&it_key);
        auto it_touch = TouchEvent::Iterator();
        touchEvents.clear(&it_touch);
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
    float AndroidInput::getAzimuth(){
        if (!compassAvailable && !rotationVectorAvailable) return 0;
        updateOrientation();
        return azimuth;
    }
    float AndroidInput::getPitch() {
        if (!compassAvailable && !rotationVectorAvailable) return 0;
        updateOrientation();
        return pitch;
    }
    float AndroidInput::getRoll() {
        if (!compassAvailable && !rotationVectorAvailable) return 0;

        updateOrientation();
        return roll;
    }
    void AndroidInput::setOnscreenKeyboardVisible(bool visible, h7::OnscreenKeyboardType type) {
        auto pEnv = ensureJniEnv();
        auto mid = pEnv->GetStaticMethodID(_andInputClazz, "setOnscreenKeyboardVisible",
                                           SIG_KEYBOARD);
        int tType = type;
        USE_REF_OBJECT(pEnv->CallStaticVoidMethod(_andInputClazz, mid, ref, visible, tType))
    }
    Orientation AndroidInput::getNativeOrientation(){
        if(nativeOrientation == Orientation::Count){
            int rotation = getRotation();
            int info[2];
            getDisplayInfo(info);
            auto width = info[0];
            auto height = info[1];
            if (((rotation == 0 || rotation == 180) && (width >= height))
                || ((rotation == 90 || rotation == 270) && (width <= height))) {
                nativeOrientation = Orientation::Landscape;
            } else {
                nativeOrientation = Orientation::Portrait;
            }
        }
        return nativeOrientation;
    }
    int AndroidInput::getRotation() {
        auto pEnv = ensureJniEnv();
        auto mid = pEnv->GetStaticMethodID(_andInputClazz, "getRotation", SIG_getRotation);
        int result;
        USE_REF_OBJECT(result = pEnv->CallStaticIntMethod(_andInputClazz, mid, ref))
        return result;
    }
    void AndroidInput::vibrate(int milliseconds) {
        auto pEnv = ensureJniEnv();
        auto mid = pEnv->GetMethodID(_andInputClazz, "vibrate", SIG_vibrate_INT);
        USE_REF_OBJECT(pEnv->CallVoidMethod(ref, mid, milliseconds));
    }
    void AndroidInput::vibrate(long long *pattern, int len, int repeat) {
        auto pEnv = ensureJniEnv();
        auto pArray = pEnv->NewLongArray(len);
        pEnv->SetLongArrayRegion(pArray, 0, len, reinterpret_cast<const jlong *>(pattern));
        auto mid = pEnv->GetMethodID(_andInputClazz, "vibrate", SIG_vibrate);
        USE_REF_OBJECT(pEnv->CallVoidMethod(ref, mid, pArray, repeat));
    }
    void AndroidInput::cancelVibrate() {
        auto pEnv = ensureJniEnv();
        auto mid = pEnv->GetMethodID(_andInputClazz, "cancelVibrate", SIG_vibrateCancel);
        USE_REF_OBJECT(pEnv->CallVoidMethod(ref, mid))
    }
}