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


//========================== impl ============================
BaseDemo* demo;

extern "C" JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API2(initAssets, jobject ctx, jobject assetM){
   auto asm1 = AAssetManager_fromJava(env, assetM);
   entry::init(asm1);
}

extern "C" JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API2(initializeSurface, jobject src,jobject surface){
    ANativeWindow *mWindow  = ANativeWindow_fromSurface(env, surface);
    InitConfig config;
    config.window = mWindow;
    config.win_width =  ANativeWindow_getWidth(mWindow);
    config.win_height =  ANativeWindow_getHeight(mWindow);

   // demo = createDemo<HelloWorldDemo>(&config);
    demo = createDemo<CurbesDemo>(&config);
    demo->draw();
}
extern "C" JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API2(destroySurface, jobject src,jobject surface){
    if(demo){
        demo->destroy();
        delete demo;
        demo = NULL ;
    }
}

extern "C" void draw(){
    if(demo){
        demo->draw();
    }
}