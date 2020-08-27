//
// Created by Administrator on 2020/8/1 0001.
//
#include "bgfx_jni.h"
#include "base_demo.h"
#include "samples/samples.h"
#include "common.h"
#include "lua/bgfx_lua_app.h"
#include "lua/lualib.h"
#include "lua/SkLua.h"

using namespace heaven7_Bgfx_demo;

#ifdef ANDROID
#include <android/native_window_jni.h>
#include <android/native_window.h>
#include <lua/bgfx_lua_app.h>
#include "android/asset_manager_jni.h"
#include "android/asset_manager.h"
#endif

#include "luaext_java/java_env.h"

//========================== impl ============================
#define MAIN_RUNNER    "com/heaven7/android/bgfx/study/demo/NativeApi"
jclass _ApiClass = nullptr;

BaseDemo* demo;
bool _useLua = false;

//func, resultCb, ..., pCount
void java_runMain(long ptr){
    JNIEnv *pEnv = getJNIEnv();
    if(pEnv == nullptr){
        pEnv = attachJNIEnv();
    }
    jmethodID mid = pEnv->GetStaticMethodID(_ApiClass, "postMain", "(J)V");
    pEnv->CallStaticVoidMethod(_ApiClass, mid, (jlong)ptr);
}

void java_onExitRenderThread(){
    JNIEnv *pEnv = getJNIEnv();
    if(pEnv != nullptr){
        pEnv->DeleteGlobalRef(_ApiClass);
        _ApiClass = nullptr;
    }
    detachJNIEnv();
}

EC_JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API1(setUseLua, jboolean useLua){
    _useLua = useLua;
}
EC_JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API(destroyAll){
#ifndef USE_NATIVE_ACTIVITY
    entry::destroy();
#endif
}

EC_JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API2(initAssets, jobject ctx, jobject assetM){
   auto asm1 = AAssetManager_fromJava(env, assetM);
#ifndef USE_NATIVE_ACTIVITY
   entry::init(asm1);
#endif
}

EC_JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API3(initializeSurface, jobject src, jobject surface, jlong ptr){
    ANativeWindow *mWindow = ANativeWindow_fromSurface(env, surface);
    if(_useLua){
        //find class can only used in main thread
        jclass aClass = env->FindClass(MAIN_RUNNER);
        _ApiClass = static_cast<jclass>(env->NewGlobalRef(aClass));
        env->DeleteLocalRef(aClass);

        InitConfig* config = new InitConfig();
        config->window = mWindow;
        config->win_width = ANativeWindow_getWidth(mWindow);
        config->win_height = ANativeWindow_getHeight(mWindow);
        config->RunMain = java_runMain;
        config->OnExitRenderThread = java_onExitRenderThread;
        Bgfx_lua_app::startApp(ptr, config);
    } else{
        //HelloWorldDemo, CurbesDemo
        InitConfig config;
        config.window = mWindow;
        config.win_width = ANativeWindow_getWidth(mWindow);
        config.win_height = ANativeWindow_getHeight(mWindow);
        demo = createDemo<FontDemo>(&config);
        demo->startLoop();
    }
}
EC_JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API1(destroySurface, jobject src){
    if(_useLua){
        //TODO
    } else{
        if(demo){
            demo->destroy();
            delete demo;
            demo = NULL ;
        }
    }
}

static const luaL_Reg bgfx_libs[] = {
        {"bgfx_lua", luaopen_bgfx_lua},
        {"bx_lua", luaopen_bx_lua},
        {nullptr, nullptr}
};
extern "C" JNIEXPORT void JNICALL Java_com_heaven7_android_bgfx_study_demo_NativeApi_initLuaBgfx(
        JNIEnv* env, jclass clazz, jlong luaPtr){
//EC_JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API1(initLuaBgfx, jlong luaPtr){
    LOGD("_initLuaBgfx");
    lua_State *L = reinterpret_cast<lua_State *>(luaPtr);
    luaL_openlibs2(L, bgfx_libs);
}