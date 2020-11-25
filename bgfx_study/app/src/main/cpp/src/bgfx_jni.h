//
// Created by Administrator on 2020/8/1 0001.
//

#ifndef BGFX_STUDY_BGFX_JNI_H
#define BGFX_STUDY_BGFX_JNI_H

#include "bgfx/bgfx.h"
#include <bx/bx.h>
#include "jni.h"

#define EC_JNIEXPORT extern "C" JNIEXPORT
#define SURFACE_VIEW_JAVA_PREFIX                        com_heaven7_android_bgfx_study_demo
#define CONCAT(prefix, class, func)                     Java_ ## prefix ## _ ## class ## _ ## func
#define CONCAT_SURFACE(prefix, func)                    CONCAT(prefix, NativeApi, func)

#define SURFACE_VIEW_JAVA_API(func)                 CONCAT_SURFACE(SURFACE_VIEW_JAVA_PREFIX, func)(JNIEnv* env, jclass)
#define SURFACE_VIEW_JAVA_API1(func, p1)            CONCAT_SURFACE(SURFACE_VIEW_JAVA_PREFIX, func)(JNIEnv* env, jclass, p1)
#define SURFACE_VIEW_JAVA_API2(func, p1, p2)        CONCAT_SURFACE(SURFACE_VIEW_JAVA_PREFIX, func)(JNIEnv* env, jclass, p1, p2)
#define SURFACE_VIEW_JAVA_API3(func, p1, p2, p3)    CONCAT_SURFACE(SURFACE_VIEW_JAVA_PREFIX, func)(JNIEnv* env, jclass, p1, p2, p3)

EC_JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API2(initAssets, jobject ctx, jobject assetM);
EC_JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API3(initializeSurface, jobject src,jobject surface, jlong luaPtr);
EC_JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API2(destroySurface, jobject src, jlong luaPtr);
EC_JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API1(setUseLua, jboolean useLua);
EC_JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API(destroyAll);
//EC_JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API1(initLuaBgfx, jlong luaPtr);

#endif //BGFX_STUDY_BGFX_JNI_H