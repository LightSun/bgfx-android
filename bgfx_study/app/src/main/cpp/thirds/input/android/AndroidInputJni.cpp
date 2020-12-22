//
// Created by Administrator on 2020/11/24 0024.
//
#include <application/Application.h>
#include "jni.h"
#include "AndroidInputJni.h"
#include "AndroidInput.h"
#include "AndroidTouchHandler.h"
#include "../GestureContext.h"
#include "../../luaext_java/java_env.h"
#include "../../lua/bgfx_app.h"
#include "../../application/Application.h"

#define MEW_CLASS  "com/heaven7/android/hbmdx/input/MotionEventWrapper"
#define MEW_CLASS_SIG "L" MEW_CLASS ";"
#define SIG_MEV_RECYCLE "(" MEW_CLASS_SIG ")V"
#define SIG_MEV_INFO "(" MEW_CLASS_SIG "I[F)V"

#define KEY_EVENT_CLASS  "com/heaven7/android/hbmdx/input/KeyEventWrapper"
#define SIG_KEY_EVENT_RECYCLE "(" KEY_EVENT_CLASS ")V"

namespace h7{
    static jclass _mew_class = nullptr;
    static jclass _keyEvent_class = nullptr;

    KeyEventWrapper::~KeyEventWrapper() {
        if(chars){
            free(chars);
            chars = NULL;
        }
    }
    void KeyEventWrapper::allocateChars(int len){
        if(chars != nullptr){
            free(chars);
            chars = NULL;
        } else if(strlen(chars) == len){
            //no need malloc
            return;
        }
        if(len > 0 ){
            chars = static_cast<char *>(malloc(len + 1));
            chars[len- 1] = '\0';
        }
    }

    void KeyEventWrapper::jRecycle() {
        auto pEnv = getJNIEnv();
        auto mid = pEnv->GetMethodID(_keyEvent_class, "recycle", SIG_KEY_EVENT_RECYCLE);
        USE_REF_OBJECT(pEnv->CallStaticVoidMethod(_keyEvent_class, mid, ref))
        deleteRefObject();
    }

    MotionEventWrapper::MotionEventWrapper(){
        auto pEnv = ensureJniEnv();
        if(_mew_class == nullptr){
            _mew_class = getGlobalClass(pEnv, MEW_CLASS);
        }
        if(_keyEvent_class == nullptr){
            _keyEvent_class = getGlobalClass(pEnv, KEY_EVENT_CLASS);
        }
    }
    void MotionEventWrapper::jRecycle() {
        auto pEnv = getJNIEnv();
        auto mid = pEnv->GetMethodID(_mew_class, "recycle", SIG_MEV_RECYCLE);
        USE_REF_OBJECT(pEnv->CallStaticVoidMethod(_mew_class, mid, ref))
        deleteRefObject();
    }
    void MotionEventWrapper::getEventInfo(int pointerIndex) {
        auto pEnv = getJNIEnv();
        auto pArray = pEnv->NewFloatArray(LEN_MOTION_INFO);
        auto mid = pEnv->GetMethodID(_mew_class, "getInfo", SIG_MEV_INFO);
        USE_REF_OBJECT(pEnv->CallStaticVoidMethod(_mew_class, mid, ref, pointerIndex, pArray))
        pEnv->GetFloatArrayRegion(pArray, 0, LEN_MOTION_INFO, tmpArr);
    }
    extern "C"
    JNIEXPORT jlong JNICALL
    Java_com_heaven7_android_hbmdx_input_MotionEventWrapper_nAlloc(JNIEnv *env, jclass clazz) {
        MotionEventWrapper* mev = new MotionEventWrapper();
        return reinterpret_cast<jlong>(mev);
    }
    extern "C"
    JNIEXPORT void JNICALL
    Java_com_heaven7_android_hbmdx_input_MotionEventWrapper_nDealloc(JNIEnv *env, jclass clazz,
                                                                     jlong ptr) {
        MotionEventWrapper* mev = reinterpret_cast<MotionEventWrapper *>(ptr);
        delete mev;
    }
    extern "C"
    JNIEXPORT void JNICALL
    Java_com_heaven7_android_hbmdx_input_MotionEventWrapper_nSet(JNIEnv *env, jclass clazz, jlong ptr, jint action,
                                                                 jint x, jint y, jint pointer_index,
                                                                 jint pointer_id, jint pointer_count,
                                                                 jint button_state, jfloat pressure,
                                                                 jint source, jint mouseDx, jint mouseDy) {
        MotionEventWrapper* mev = reinterpret_cast<MotionEventWrapper *>(ptr);
        mev->action = action;
        mev->x = x;
        mev->y = y;
        mev->pointerIndex = pointer_index;
        mev->pointerId = pointer_id;
        mev->pointerCount = pointer_count;
        mev->buttonState = button_state;
        mev->pressure = pressure;
        mev->timeStamp = getCurrentTime();
        //often for mouse
        mev->scrollAmountX = mouseDx;
        mev->scrollAmountY = mouseDy;
        mev->source = source;
    }
    extern "C"
    JNIEXPORT void JNICALL
    Java_com_heaven7_android_hbmdx_input_AndroidInput_nOnTouch(JNIEnv *env, jclass clazz,jlong inputPtr,
                                                               jlong me_ptr, jobject jWrapper) {
        AndroidInput* input = reinterpret_cast<AndroidInput *>(inputPtr);
        MotionEventWrapper* mev = reinterpret_cast<MotionEventWrapper *>(me_ptr);
        mev->setRefObject(jWrapper);
        android_onTouch(input, mev);
    }
    extern "C"
    JNIEXPORT jboolean JNICALL
    Java_com_heaven7_android_hbmdx_input_AndroidInput_nOnKey(JNIEnv *env, jclass clazz, jlong input_ptr,
                                                             jlong ke_ptr, jint key_code,
                                                             jobject wrapper) {
        AndroidInput* input = reinterpret_cast<AndroidInput *>(input_ptr);
        KeyEventWrapper* mev = reinterpret_cast<KeyEventWrapper *>(ke_ptr);
        mev->setRefObject(wrapper);
        return sCast(jboolean, android_onKey(input, key_code, mev));
    }
    extern "C"
    JNIEXPORT jboolean JNICALL
    Java_com_heaven7_android_hbmdx_input_AndroidInput_nOnGenericMotion(JNIEnv *env, jclass clazz,
                                                                       jlong inputPtr, jlong me_ptr,
                                                                       jobject wrapper) {
        AndroidInput* input = reinterpret_cast<AndroidInput *>(inputPtr);
        MotionEventWrapper* mev = reinterpret_cast<MotionEventWrapper *>(me_ptr);
        mev->setRefObject(wrapper);
        return sCast(jboolean, android_onGenericMotion(input, mev));
    }
    extern "C"
    JNIEXPORT jlong JNICALL
    Java_com_heaven7_android_hbmdx_input_AndroidInput_nAlloc(JNIEnv *env, jclass clazz,jobject obj) {
        AndroidInput* mev = new AndroidInput();
        mev->setRefObject(obj);
        Application::get()->input = mev;
        return reinterpret_cast<jlong>(mev);
    }
    extern "C"
    JNIEXPORT void JNICALL
    Java_com_heaven7_android_hbmdx_input_AndroidInput_nDealloc(JNIEnv *env, jclass clazz, jlong ptr) {
        AndroidInput* mev = reinterpret_cast<AndroidInput *>(ptr);
        delete mev;
    }
    extern "C"
    JNIEXPORT jlong JNICALL
    Java_com_heaven7_android_hbmdx_input_KeyEventWrapper_nAlloc(JNIEnv *env, jclass clazz) {
        KeyEventWrapper* ptr = new KeyEventWrapper();
        return reinterpret_cast<jlong>(ptr);
    }
    extern "C"
    JNIEXPORT void JNICALL
    Java_com_heaven7_android_hbmdx_input_KeyEventWrapper_nDealloc(JNIEnv *env, jclass clazz,
                                                                  jlong ptr) {
        KeyEventWrapper* k = reinterpret_cast<KeyEventWrapper *>(ptr);
        delete k;
    }
    extern "C"
    JNIEXPORT void JNICALL
    Java_com_heaven7_android_hbmdx_input_KeyEventWrapper_nSet(JNIEnv *env, jclass clazz, jlong ptr,
                                                              jint action, jint repeat_count,
                                                              jint key_code, jstring chars,
                                                              jint unicode_char,
                                                              jboolean alt_pressed) {
        KeyEventWrapper* kew = rCast(KeyEventWrapper*, ptr);
        kew->action = action;
        kew->repeatCount = repeat_count;
        kew->keyCode = key_code;
        kew->timeStamp = getCurrentTime();
        kew->unicodeChar = unicode_char;
        kew->altPressed = alt_pressed;
        if(chars != nullptr){
            auto len = env->GetStringLength(chars);
            kew->allocateChars(len);
            env->GetStringUTFRegion(chars, 0, len, kew->chars);
        } else{
            kew->allocateChars(0);
        }
    }

    extern "C"
    JNIEXPORT void JNICALL
    Java_com_heaven7_android_hbmdx_input_AndroidInput_nSetSensorAvailables(JNIEnv *env, jclass clazz,
                                                                           jlong ptr,
                                                                           jboolean accelerometer,
                                                                           jboolean gyroscope,
                                                                           jboolean rotation_vector,
                                                                           jboolean compass) {
        AndroidInput* input = rCast(AndroidInput*, ptr);
        input->accelerometerAvailable = accelerometer;
        input->gyroscopeAvailable = gyroscope;
        input->rotationVectorAvailable = rotation_vector;
        input->compassAvailable = compass;
    }

    extern "C"
    JNIEXPORT void JNICALL
    Java_com_heaven7_android_hbmdx_input_AndroidInput_nAccelerometer_1Changed(JNIEnv *env, jclass clazz,
                                                                              jlong ptr,
                                                                              jfloatArray values) {
        AndroidInput* input = rCast(AndroidInput*, ptr);
        auto len = env->GetArrayLength(values);
        if(input->getNativeOrientation() == Orientation::Portrait){
             env->GetFloatArrayRegion(values, 0, len, input->accelerometerValues);
        } else{
            float arr[len];
            env->GetFloatArrayRegion(values, 0, len, arr);
            input->accelerometerValues[0] = arr[1];
            input->accelerometerValues[1] = -arr[0];
            input->accelerometerValues[2] = arr[2];
        }
    }
    extern "C"
    JNIEXPORT void JNICALL
    Java_com_heaven7_android_hbmdx_input_AndroidInput_nMagnetic_1field_1Changed(JNIEnv *env,
                                                                                jclass clazz, jlong ptr,
                                                                                jfloatArray values) {
        AndroidInput* input = rCast(AndroidInput*, ptr);
        auto len = env->GetArrayLength(values);
        env->GetFloatArrayRegion(values, 0, len, input->magneticFieldValues);
    }
    extern "C"
    JNIEXPORT void JNICALL
    Java_com_heaven7_android_hbmdx_input_AndroidInput_nGyroscope_1Changed(JNIEnv *env, jclass clazz,
                                                                          jlong ptr,
                                                                          jfloatArray values) {
        AndroidInput* input = rCast(AndroidInput*, ptr);
        auto len = env->GetArrayLength(values);
        if(input->getNativeOrientation() == Orientation::Portrait){
            env->GetFloatArrayRegion(values, 0, len, input->gyroscopeValues);
        } else{
            float arr[len];
            env->GetFloatArrayRegion(values, 0, len, arr);
            input->gyroscopeValues[0] = arr[1];
            input->gyroscopeValues[1] = -arr[0];
            input->gyroscopeValues[2] = arr[2];
        }
    }
    extern "C"
    JNIEXPORT void JNICALL
    Java_com_heaven7_android_hbmdx_input_AndroidInput_nRotation_1vector_1Changed(JNIEnv *env,
                                                                                jclass clazz, jlong ptr,
                                                                                jfloatArray values) {
        AndroidInput* input = rCast(AndroidInput*, ptr);
        auto len = env->GetArrayLength(values);
        if(input->getNativeOrientation() == Orientation::Portrait){
            env->GetFloatArrayRegion(values, 0, len, input->rotationVectorValues);
        } else{
            float arr[len];
            env->GetFloatArrayRegion(values, 0, len, arr);
            input->rotationVectorValues[0] = arr[1];
            input->rotationVectorValues[1] = -arr[0];
            input->rotationVectorValues[2] = arr[2];
        }
    }
}
