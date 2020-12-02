//
// Created by Administrator on 2020/8/2 0002.
//

#ifndef BGFX_STUDY_PLAT_CONTEXT_H
#define BGFX_STUDY_PLAT_CONTEXT_H

#ifdef ANDROID
#include "android/asset_manager_jni.h"
#include "android/asset_manager.h"
#include <android/native_window_jni.h>
#include <android/native_window.h>
#endif

namespace Platforms{
    void releaseWindow(void* window);

    long long getLuaPtr();

    void initLuaBgfx();
};



#endif //BGFX_STUDY_PLAT_CONTEXT_H
