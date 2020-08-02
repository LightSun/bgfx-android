//
// Created by Administrator on 2020/8/1 0001.
//
#include "bgfx_view.h"

#ifdef ANDROID
#include <android/native_window_jni.h>
#include <android/native_window.h>
#endif

//========================== impl ============================
int m_width;
int m_height;

JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API1(initializeSurface, jobject surface){
    // surface 来自于上层
    ANativeWindow *mWindow  = ANativeWindow_fromSurface(env, surface);

    bgfx::Init init;
// 选择一个渲染后端，当设置为 RendererType::Enum::Count 的时候，系统将默认选择一个平台，可以设置Metal，OpenGL ES，Direct 等
    init.type = bgfx::RendererType::Enum::Count;
// 设置供应商接口Vendor PCI ID，默认设置为0将选择第一个设备来显示。
// #define BGFX_PCI_ID_NONE                UINT16_C(0x0000) //!< Autoselect adapter.
// #define BGFX_PCI_ID_SOFTWARE_RASTERIZER UINT16_C(0x0001) //!< Software rasterizer.
// #define BGFX_PCI_ID_AMD                 UINT16_C(0x1002) //!< AMD adapter.
// #define BGFX_PCI_ID_INTEL               UINT16_C(0x8086) //!< Intel adapter.
// #define BGFX_PCI_ID_NVIDIA              UINT16_C(0x10de) //!< nVidia adapter.
    init.vendorId = 0;
// 设置分辨率大小
    init.resolution.width = m_width = ANativeWindow_getWidth(mWindow);
    init.resolution.height = m_height = ANativeWindow_getHeight(mWindow);
// BGFX_RESET_VSYNC 其作用主要是让显卡的运算和显示器刷新率一致以稳定输出的画面质量。
    init.resolution.reset = BGFX_RESET_VSYNC;
    // 设置平台信息，绑定上层 view
    init.platformData.nwh = mWindow;
    init.platformData.ndt = NULL;
    init.platformData.context = NULL;
    init.platformData.backBuffer = NULL;
    init.platformData.backBufferDS = NULL;
    bgfx::init(init);

   // Enable debug text.
    bgfx::setDebug(BGFX_DEBUG_TEXT);

    // Set view 0 clear state.
    bgfx::setViewClear(0
            , BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
            , 0x303030ff
            , 1.0f
            , 0
    );
    draw();
}
JNIEXPORT void JNICALL SURFACE_VIEW_JAVA_API1(destroySurface, jobject surface){
    bgfx::shutdown();
}

void draw(){
    // Set view 0 default viewport.
    bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height) );

    // This dummy draw call is here to make sure that view 0 is cleared
    // if no other draw calls are submitted to view 0.
    bgfx::touch(0);

    // Use debug font to print information about this example.
    bgfx::dbgTextClear();
    bgfx::dbgTextImage(
            bx::max<uint16_t>(uint16_t(m_width /2/8 ), 20)-20
            , bx::max<uint16_t>(uint16_t(m_height/2/16),  6)-6
            , 40
            , 12
            , s_logo
            , 160
    );
    bgfx::dbgTextPrintf(0, 1, 0x0f, "Color can be changed with ANSI \x1b[9;me\x1b[10;ms\x1b[11;mc\x1b[12;ma\x1b[13;mp\x1b[14;me\x1b[0m code too.");

    bgfx::dbgTextPrintf(80, 1, 0x0f, "\x1b[;0m    \x1b[;1m    \x1b[; 2m    \x1b[; 3m    \x1b[; 4m    \x1b[; 5m    \x1b[; 6m    \x1b[; 7m    \x1b[0m");
    bgfx::dbgTextPrintf(80, 2, 0x0f, "\x1b[;8m    \x1b[;9m    \x1b[;10m    \x1b[;11m    \x1b[;12m    \x1b[;13m    \x1b[;14m    \x1b[;15m    \x1b[0m");

    const bgfx::Stats* stats = bgfx::getStats();
    bgfx::dbgTextPrintf(0, 2, 0x0f, "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters."
            , stats->width
            , stats->height
            , stats->textWidth
            , stats->textHeight
    );

    // Advance to next frame. Rendering thread will be kicked to
    // process submitted rendering primitives.
    bgfx::frame();
}