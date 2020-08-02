//
// Created by Administrator on 2020/8/1 0001.
//

#ifndef BGFX_STUDY_BGFX_VIEW_H
#define BGFX_STUDY_BGFX_VIEW_H

#include "bgfx/bgfx.h"
#include <bx/bx.h>
#include "logo.h"
#include "jni.h"

#define SURFACE_VIEW_JAVA_PREFIX                        com_heaven7_android_bgfx_study_demo
#define CONCAT(prefix, class, func)                     Java_ ## prefix ## _ ## class ## _ ## func
#define CONCAT_SURFACE(prefix, func)                    CONCAT(prefix, BgfxSurfaceView,func)

#define SURFACE_VIEW_JAVA_API(function)                 CONCAT_SURFACE(SURFACE_VIEW_JAVA_PREFIX, function)(JNIEnv* env, jobject jobj)
#define SURFACE_VIEW_JAVA_API1(function, p1)            CONCAT_SURFACE(SURFACE_VIEW_JAVA_PREFIX, function)(JNIEnv* env, jobject jobj, p1)
#define SURFACE_VIEW_JAVA_API2(function, p1, p2)        CONCAT_SURFACE(SURFACE_VIEW_JAVA_PREFIX, function)(JNIEnv* env, jobject jobj, p1, p2)

JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API1(initializeSurface, jobject surface);
JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API1(destroySurface, jobject surface);
void draw();

// 封装顶点对象
struct PosColorVertex {
    // 顶点坐标
    float m_x;
    float m_y;
    float m_z;
    // 纹理坐标
    int16_t m_u;
    int16_t m_v;
    // 顶点描述对象
    static bgfx::VertexLayout ms_decl;

    static void init() {
        // 这句话的意思是位置数据里面，前三个 Float 类型是作为顶点坐标，后两个 Int16 类的值作为纹理的坐标
        ms_decl
                .begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Int16, true)
                .end();
    };
};

// 这个地方要注意了，此时 FBO 的纹理坐标 Android 和 iOS 都是采用左下角作为纹理坐标原点，
// iOS 或者 Mac 平台在渲染的时候，也是使用同样的坐标来渲染，但是 Android 平台不一样，
// Android 平台在渲染纹理的时候，是采用左上角作为纹理坐标来渲染的,
// 所以对于 Android 平台来说，下面还需要一个渲染的坐标 s_Android_render_Vertices1
static PosColorVertex s_fbo_Vertices[] =
        {
                {-1.0f,  1.0f,  0.0f,      0, 0x7fff},
                { 1.0f,  1.0f,  0.0f, 0x7fff, 0x7fff},
                {-1.0f, -1.0f,  0.0f,      0,      0},
                { 1.0f, -1.0f,  0.0f, 0x7fff,      0},
        };

// Android 平台渲染的坐标和纹理顶点，左上角为纹理原点
static PosColorVertex s_Android_render_Vertices1[] =
        {
                {-1.0f,  1.0f,  0.0f,      0,      0},
                { 1.0f,  1.0f,  0.0f, 0x7fff,      0},
                {-1.0f, -1.0f,  0.0f,      0, 0x7fff},
                { 1.0f, -1.0f,  0.0f, 0x7fff, 0x7fff},
        };

// 顶点绘制顺序
static const uint16_t s_TriList[] =
        {
                0, 2, 1,
                1, 2, 3,
        };

#endif //BGFX_STUDY_BGFX_VIEW_H
