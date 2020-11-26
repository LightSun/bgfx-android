//
// Created by Administrator on 2020/8/2 0002.
//

#include "android_pri.h"
#ifdef ANDROID
#include <android/native_window_jni.h>
#include <android/native_window.h>
#include "android/asset_manager_jni.h"
#include "android/asset_manager.h"
#endif

namespace Platforms{

    void releaseWindow(void* window){
#ifdef ANDROID
        ANativeWindow* anw = static_cast<ANativeWindow *>(window);
        ANativeWindow_release(anw);
#endif
    }
}