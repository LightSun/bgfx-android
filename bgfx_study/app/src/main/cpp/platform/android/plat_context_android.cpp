//
// Created by Administrator on 2020/12/2 0002.
//

#include <android/native_window_jni.h>
#include <android/native_window.h>
#include "android/asset_manager_jni.h"
#include "android/asset_manager.h"

#include "jni.h"
#include "common.h"
#include "../plat_context.h"

static long long _luaPtr;

void Platforms::releaseWindow(void *window) {
    ANativeWindow* anw = static_cast<ANativeWindow *>(window);
    ANativeWindow_release(anw);
}

long long Platforms::getLuaPtr(){
    return _luaPtr;
}

namespace heaven7{

    extern "C"
    JNIEXPORT void JNICALL
    Java_com_heaven7_android_hbmdx_Heaven7_nInit(JNIEnv *env, jclass clazz, jobject context, jobject assetM,
                                                 jlong lua_ptr) {
        _luaPtr = lua_ptr;
        auto asm1 = AAssetManager_fromJava(env, assetM);
#ifndef USE_NATIVE_ACTIVITY
        entry::init(asm1);
#endif
        Platforms::initLuaBgfx();
    }
    extern "C"
    JNIEXPORT void JNICALL
    Java_com_heaven7_android_hbmdx_Heaven7_nDestroy(JNIEnv *env, jclass clazz) {
#ifndef USE_NATIVE_ACTIVITY
        entry::destroy();
#endif
    }
}