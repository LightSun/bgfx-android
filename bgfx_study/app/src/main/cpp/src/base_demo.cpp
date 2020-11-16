//
// Created by Administrator on 2020/8/2 0002.
//

#include "base_demo.h"

namespace heaven7_Bgfx_demo{
    BaseDemo::BaseDemo() {

    }
    void BaseDemo::setInitConfig(InitConfig *c){
        this->config = *c;
    }
    void BaseDemo::init() {
        destroyed = false;
      /*  m_reset = BGFX_RESET_VSYNC;
        m_debug = BGFX_DEBUG_TEXT;*/

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
        init.resolution.width = config.win_width;
        init.resolution.height = config.win_height;
// BGFX_RESET_VSYNC 其作用主要是让显卡的运算和显示器刷新率一致以稳定输出的画面质量。
        init.resolution.reset = m_reset;
        // 设置平台信息，绑定上层 view
        init.platformData.nwh = config.window;
        init.platformData.ndt = NULL;
        init.platformData.context = NULL;
        init.platformData.backBuffer = NULL;
        init.platformData.backBufferDS = NULL;
        init.callback = m_callback;
        init.allocator = m_allocator;
        bgfx::init(init);

        // Enable debug text.
        bgfx::setDebug(m_debug);

        // Set view 0 clear state.
        bgfx::setViewClear(0
                , BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
                , 0x303030ff
                , 1.0f
                , 0
        );
    }

    void BaseDemo::destroy() {
        if(!destroyed){
            destroyed = true;
            m_thread.shutdown();
            Platforms::releaseWindow(config.window);
            bgfx::shutdown();
        }
    }
    bool BaseDemo::isDestroyed() {
        return destroyed;
    }
    void BaseDemo::startLoop() {
        if (!m_thread.isRunning() )
        {
            m_thread.init(BaseDemo::threadFunc, this);
        }
    }
    int32_t BaseDemo::threadFunc(bx::Thread* _thread, void* _userData)
    {
        BX_UNUSED(_thread);
        //bgfx init and draw must in one thread.
        BaseDemo* demo = static_cast<BaseDemo *>(_userData);
        demo->init();
        bgfx::frame();
        //int32_t result = chdir("/sdcard/bgfx/examples/runtime");
        // BX_ASSERT(0 == result, "Failed to chdir to dir. android.permission.WRITE_EXTERNAL_STORAGE?", errno);
        DBG("loop draw >>> start");
        int i = 0;
        while(demo->draw() == 0){
           // DBG("loop draw >>> %d", ++i);
        }
        DBG("loop draw >>> end");
//		PostMessage(s_ctx.m_hwnd, WM_QUIT, 0, 0);
        return 0;
    }
}
