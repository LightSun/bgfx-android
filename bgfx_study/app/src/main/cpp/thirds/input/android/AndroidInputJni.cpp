//
// Created by Administrator on 2020/11/24 0024.
//
#include "jni.h"
#include "AndroidInputJni.h"
#include "AndroidInput.h"
#include "AndroidTouchHandler.h"

namespace h7{
    float MotionEventWrapper::getX(int pointerIndex) {

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
                                                               jlong me_ptr) {
        AndroidInput* input = reinterpret_cast<AndroidInput *>(inputPtr);
        MotionEventWrapper* mev = reinterpret_cast<MotionEventWrapper *>(me_ptr);
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
}
