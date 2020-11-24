//
// Created by Administrator on 2020/11/24 0024.
//
#include "jni.h"
#include "AndroidInputJni.h"
#include "AndroidInput.h"
#include "AndroidTouchHandler.h"
#include "../../luaext_java/java_env.h"

#define MEW_CLASS  "com/heaven7/android/hbmdx/input/MotionEventWrapper"
#define MEW_CLASS_SIG "L" MEW_CLASS ";"
#define SIG_GET_F "(" MEW_CLASS_SIG "I)F"
#define SIG_GET_I "(" MEW_CLASS_SIG "I)I"

#define USE_REF_OBJECT(x) \
do{\
 auto ref = getRefObject();\
 auto result = x;\
 pEnv->DeleteLocalRef(ref);\
 return result; \
} while(0);


namespace h7{
    static jclass _mew_class = nullptr;
    MotionEventWrapper::MotionEventWrapper():WeakObjectM() {
        if(_mew_class == nullptr){
            _mew_class = getGlobalClass(getJNIEnv(), MEW_CLASS);
        }
    }
    float MotionEventWrapper::getX(int pointerIndex) {
        auto pEnv = getJNIEnv();
        auto mid = pEnv->GetMethodID(_mew_class, "getX", SIG_GET_F);
        USE_REF_OBJECT(pEnv->CallStaticFloatMethod(_mew_class, mid, ref, pointerIndex))
    }
    float MotionEventWrapper::getY(int pointerIndex) {
        auto pEnv = getJNIEnv();
        auto mid = pEnv->GetMethodID(_mew_class, "getY", SIG_GET_F);
        USE_REF_OBJECT(pEnv->CallStaticFloatMethod(_mew_class, mid, ref, pointerIndex))
    }
    float MotionEventWrapper::getPressure(int pointerIndex) {
        auto pEnv = getJNIEnv();
        auto mid = pEnv->GetMethodID(_mew_class, "getPressure", SIG_GET_F);
        USE_REF_OBJECT(pEnv->CallStaticFloatMethod(_mew_class, mid, ref, pointerIndex))
    }
    int MotionEventWrapper::getPointerId(int pointerIndex) {
        auto pEnv = getJNIEnv();
        auto mid = pEnv->GetMethodID(_mew_class, "getPointerId", SIG_GET_I);
        USE_REF_OBJECT(pEnv->CallStaticIntMethod(_mew_class, mid, ref, pointerIndex))
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
                                                                 jlong time_stamp) {
        MotionEventWrapper* mev = reinterpret_cast<MotionEventWrapper *>(ptr);
        mev->action = action;
        mev->x = x;
        mev->y = y;
        mev->pointerIndex = pointer_index;
        mev->pointerId = pointer_id;
        mev->pointerCount = pointer_count;
        mev->buttonState = button_state;
        mev->pressure = pressure;
        mev->timeStamp = time_stamp;
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
    JNIEXPORT jlong JNICALL
    Java_com_heaven7_android_hbmdx_input_AndroidInput_nAlloc(JNIEnv *env, jclass clazz) {
        AndroidInput* mev = new AndroidInput();
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
}
