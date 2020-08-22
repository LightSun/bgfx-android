//
// Created by Administrator on 2020/8/1 0001.
//
#include "bgfx_jni.h"
#include "base_demo.h"
#include "samples/samples.h"
#include "common.h"
#include "bgfx_lua_app.h"

using namespace heaven7_Bgfx_demo;

#ifdef ANDROID
#include <android/native_window_jni.h>
#include <android/native_window.h>
#include <lua/bgfx_lua_app.h>
#include "android/asset_manager_jni.h"
#include "android/asset_manager.h"
#endif

//========================== impl ============================
BaseDemo* demo;
bool _useLua = false;

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

EC_JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API2(initializeSurface, jobject src,jobject surface){
    ANativeWindow *mWindow = ANativeWindow_fromSurface(env, surface);
    if(_useLua){
        Bgfx_lua_app::initPlatformData(mWindow);
    } else{
        InitConfig config;
        config.window = mWindow;
        config.win_width =  ANativeWindow_getWidth(mWindow);
        config.win_height =  ANativeWindow_getHeight(mWindow);

        //HelloWorldDemo, CurbesDemo
        demo = createDemo<FontDemo>(&config);
        demo->startLoop();
    }
}
EC_JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API1(destroySurface, jobject src){
    if(_useLua){
        Bgfx_lua_app::destroyLuaApp();
    } else{
        if(demo){
            demo->destroy();
            delete demo;
            demo = NULL ;
        }
    }
}