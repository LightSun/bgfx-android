//
// Created by Administrator on 2020/11/24 0024.
//

#ifndef BGFX_STUDY_ANDROIDINPUT_H
#define BGFX_STUDY_ANDROIDINPUT_H

#include <mutex>
#include "../Input.h"
#include "../../utils/Pool.hpp"
#include "IAndroidInput.h"
#include "jni_base.hpp"

#define LOCK_UNTIL_RESULT(x)\
        do{\
            lockTouch();\
            auto result = x;\
            unlockTouch();\
            return result;\
        }while(0);

namespace h7 {
    //TODO latter impl Input
    class AndroidInput: public Input,IAndroidInput,WeakObjectM {

    public:
        const static int NUM_TOUCHES = 20;
        const static int SUPPORTED_KEYS = 260;
        const static int NUM_PRESS_BUTTON = 20;
        int *touchX;
        int *touchY;
        int *deltaX;
        int *deltaY;
        bool *touched;

        int *button;
        int *realId;
        float *pressure;

        bool hasMultitouch;
        int keyCount = 0;

        int len_realId = NUM_TOUCHES;

        bool keyJustPressed = false;
        bool keys[SUPPORTED_KEYS];
        bool justPressedKeys[SUPPORTED_KEYS];
        bool justPressedButtons[NUM_PRESS_BUTTON];

        Pool<KeyEvent> usedKeyEvents = Pool<KeyEvent>(16, 100);
        Pool<TouchEvent> usedTouchEvents = Pool<TouchEvent>(16, 100);;

        Array<OnKeyListener *> keyListeners;
        Array<KeyEvent *> keyEvents;
        Array<TouchEvent *> touchEvents;

        InputProcessor* _inputProcessor;
        Array<OnGenericMotionListener*> genericMotionListeners;

        bool gyroscopeAvailable = false;
        bool accelerometerAvailable = false;
        bool compassAvailable = false;
        bool keyboardAvailable = false;
        bool rotationVectorAvailable = false;
        float accelerometerValues[3];
        float gyroscopeValues[3];
         // the key to catch
        IntArray keysToCatch;

        Orientation nativeOrientation = Orientation::Count;
        bool _justTouched = false;
        long long currentEventTimeStamp = 0;

        float orientation[3];
        float azimuth = 0;
        float pitch = 0;
        float roll = 0;

        float magneticFieldValues[3];
        float rotationVectorValues[3];
        float R[9];

        AndroidInput();

        ~AndroidInput();

        inline void setRefObject(jobject obj){
            WeakObjectM::setRefObject(obj);
        }
        void processEvents ();
        int getFreePointerIndex();

        int lookUpPointerIndex(int pointerId);

        bool isPeripheralAvailable (Peripheral peripheral);
        bool isKeyPressed (int key);
        bool isKeyJustPressed (int key);
        void setCatchKey (int keycode, bool catchKey);

        inline void setCatchBackKey (bool catchBack){
            setCatchKey(Keys::BACK, catchBack);
        }
        inline bool isCatchBackKey() {
            return keysToCatch.contains(cCast_ref(int, Keys::BACK));
        }
        inline bool isCatchKey (int keycode){
            return keysToCatch.contains(keycode);
        }
        inline void setCatchMenuKey (bool catchMenu){
            setCatchKey(Keys::MENU, catchMenu);
        }
        inline bool isCatchMenuKey () {
            return keysToCatch.contains(cCast_ref(int, Keys::MENU));
        }
        inline bool justTouched () {
            return _justTouched;
        }
        int getRotation();
        Orientation getNativeOrientation();

        void vibrate (int milliseconds);
        void vibrate (long long* pattern, int len,int repeat);
        void cancelVibrate();

        /** Returns the rotation matrix describing the devices rotation as per <a href=
	 * "http://developer.android.com/reference/android/hardware/SensorManager.html#getRotationMatrix(float[], float[], float[], float[])"
	 * >SensorManager#getRotationMatrix(float[], float[], float[], float[])</a>. Does not manipulate the matrix if the platform
	 * does not have an accelerometer and compass, or a rotation vector sensor.
         * the len of matrix must be 9.
	 * @param matrix */
       void getRotationMatrix(float* matrix);

        float getAzimuth ();
        float getPitch ();
        float getRoll ();

        bool isButtonPressed (int button);

        inline bool isButtonJustPressed(int button) {
            if(button < 0 || button > NUM_TOUCHES) return false;
            return justPressedButtons[button];
        }
        void setOnscreenKeyboardVisible(bool visible, OnscreenKeyboardType type);

        inline void setOnscreenKeyboardVisible (bool visible){
            setOnscreenKeyboardVisible(visible, OnscreenKeyboardType::Default);
        }
        inline void addKeyListener(OnKeyListener* listener) {
            keyListeners.add(listener);
        }
        inline void addGenericMotionListener (OnGenericMotionListener* listener){
            genericMotionListeners.add(listener);
        }
        inline void setInputProcessor(InputProcessor* processor){
            this->_inputProcessor = processor;
        }
        inline InputProcessor* getInputProcessor (){
            return _inputProcessor;
        }
        inline void setKeyboardAvailable (bool available){
            this->keyboardAvailable = available;
        }
        inline float getPressure(int pointer){
            LOCK_UNTIL_RESULT(pressure[pointer]);
        }
        inline float getPressure(){
            LOCK_UNTIL_RESULT(pressure[0]);
        }
        inline bool isTouched(int pointer){
            LOCK_UNTIL_RESULT(touched[pointer]);
        }
        bool isTouched();
        inline int getX() {
            LOCK_UNTIL_RESULT(touchX[0]);
        }
        inline int getY() {
            LOCK_UNTIL_RESULT(touchY[0]);
        }
        inline int getX(int pointer) {
            LOCK_UNTIL_RESULT(touchX[pointer]);
        }
        inline int getY(int pointer) {
            LOCK_UNTIL_RESULT(touchY[pointer]);
        }
        inline int getMaxPointers() {
            return NUM_TOUCHES;
        }
        inline float getAccelerometerX() {
            return accelerometerValues[0];
        }
        inline float getAccelerometerY() {
            return accelerometerValues[1];
        }
        inline float getAccelerometerZ() {
            return accelerometerValues[2];
        }
        inline float getGyroscopeX() {
            return gyroscopeValues[0];
        }
        inline float getGyroscopeY() {
            return gyroscopeValues[1];
        }
        inline float getGyroscopeZ() {
            return gyroscopeValues[2];
        }
        inline int getDeltaX(){
            return deltaX[0];
        }
        inline int getDeltaX(int pointerIndex){
            return deltaX[pointerIndex];
        }
        inline int getDeltaY(){
            return deltaY[0];
        }
        inline int getDeltaY(int pointerIndex){
            return deltaY[pointerIndex];
        }
        inline long long getCurrentEventTime () {
            return currentEventTimeStamp;
        }
        inline void setCursorCatched(bool catched){
        }
        inline bool isCursorCatched(){
            return false;
        }
        void setCursorPosition(int x, int y){
        }
        inline void lockTouch() {
            _mutex.lock();
        }
        inline void unlockTouch() {
            _mutex.unlock();
        }

    private:
        std::mutex _mutex;

        inline void release();

        inline void ensureSize(int expect);

        inline void updateOrientation();

        inline bool getRotationMatrix0(jfloatArray _R, float *mat_outR);
    };


}

#endif //BGFX_STUDY_ANDROIDINPUT_H
