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

#define SIG_MEV_RECYCLE "(" MEW_CLASS_SIG ")V"
#define SIG_MEV_INFO "(" MEW_CLASS_SIG "I[F)V"

#define USE_REF_OBJECT(x) \
do{\
 auto ref = getRefObject();\
 x;\
 pEnv->DeleteLocalRef(ref);\
} while(0);


namespace h7{
    static jclass _mew_class = nullptr;
    MotionEventWrapper::MotionEventWrapper(){
        if(_mew_class == nullptr){
            _mew_class = getGlobalClass(getJNIEnv(), MEW_CLASS);
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
    extern "C"
    JNIEXPORT void JNICALL
    Java_com_heaven7_android_hbmdx_input_KeyEventWrapper_nSet(JNIEnv *env, jclass clazz, jlong ptr,
                                                              jint action, jint repeat_count,
                                                              jint key_code, jstring chars,
                                                              jlong time_stamp, jint unicode_char,
                                                              jboolean alt_pressed) {
        KeyEventWrapper* kew = rCast(KeyEventWrapper*, ptr);
        kew->action = action;
        kew->repeatCount = repeat_count;
        kew->keyCode = key_code;
        kew->timeStamp = time_stamp;
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
}
