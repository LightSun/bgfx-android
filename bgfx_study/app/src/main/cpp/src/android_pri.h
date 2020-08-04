//
// Created by Administrator on 2020/8/2 0002.
//

#ifndef BGFX_STUDY_ANDROID_PRI_H
#define BGFX_STUDY_ANDROID_PRI_H

#ifdef ANDROID
#include "android/asset_manager_jni.h"
#include "android/asset_manager.h"
#include <android/native_window_jni.h>
#include <android/native_window.h>
#endif

extern "C"{
void releaseWindow(void* window);

};



#endif //BGFX_STUDY_ANDROID_PRI_H
