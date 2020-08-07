//
// Created by Administrator on 2020/8/1 0001.
//
#include "bgfx_jni.h"
#include "base_demo.h"
#include "samples/samples.h"
#include "common.h"

using namespace heaven7_Bgfx_demo;

#ifdef ANDROID
#include <android/native_window_jni.h>
#include <android/native_window.h>
#include "android/asset_manager_jni.h"
#include "android/asset_manager.h"
#endif

bx::Thread m_thread;

//========================== impl ============================
BaseDemo* demo;

EC_JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API2(initAssets, jobject ctx, jobject assetM){
   auto asm1 = AAssetManager_fromJava(env, assetM);
#ifndef USE_NATIVE_ACTIVITY
   entry::init(asm1);
#endif
}

EC_JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API2(initializeSurface, jobject src,jobject surface){
    ANativeWindow *mWindow  = ANativeWindow_fromSurface(env, surface);
    InitConfig config;
    config.window = mWindow;
    config.win_width =  ANativeWindow_getWidth(mWindow);
    config.win_height =  ANativeWindow_getHeight(mWindow);

   // demo = createDemo<HelloWorldDemo>(&config);
    demo = createDemo<CurbesDemo>(&config);
    demo->startLoop();
}
EC_JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API2(destroySurface, jobject src,jobject surface){
    if(demo){
        demo->destroy();
        delete demo;
        demo = NULL ;
    }
}
EC_JNIEXPORT jboolean JNICALL SURFACE_VIEW_JAVA_API(update){
    return draw() == 0;
}
extern "C" int draw(){
    if(demo){
        return demo->draw();
    }
    return -1;
}