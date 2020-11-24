//
// Created by Administrator on 2020/11/24 0024.
//

#include <mutex>

#include "bgfx/bgfx.h"
#include "bgfx_wrapper.h"
#include "bgfx_app.h"

#include <log.h>
#include "../core/common.h"

namespace h7{
    void BgfxApp::doInit(bgfx::Init *pInit, entry::InitConfig *pConfig) {
        onPreInit();
        //LOGD("holder = %p, init = %p, resolution = %p", holder, pInit, &pInit->resolution);
        //LOGD("init config, w = %d, h = %d", pConfig->win_width, pConfig->win_height);
        pInit->resolution.width = pConfig->win_width;
        pInit->resolution.height = pConfig->win_height;

        pInit->platformData.nwh = pConfig->window;
        pInit->platformData.ndt = NULL;
        pInit->platformData.context = NULL;
        pInit->platformData.backBuffer = NULL;
        pInit->platformData.backBufferDS = NULL;
        bgfx::init(*pInit);
        //LOGD("BgfxApp init is called. holder = %p,func_init= %s ", holder, func_init);
        onInit();
    }
    void BgfxApp::actDestroy(bool lightly) {
        if(lightly){
            onDestroy(lightly);
            bgfx::shutdown();
        } else{
            if (getState() != APP_STATE_DESTROYED) {
                setState(APP_STATE_DESTROYED);
                ext_println("destroy");
                onDestroy(lightly);
                //destroy thread. window , and bgfx
                //m_thread.shutdown();
                bgfx::shutdown();
            }
        }
    }

    unsigned char BgfxApp::getState() {
            return state.load(std::memory_order_relaxed);
    }
    void BgfxApp::setState(unsigned char s) {
            state.store(s, std::memory_order_release);//对读线程可见
    }
    void BgfxApp::onPause() {
        LOGW("onPause");
    }
    void BgfxApp::onResume() {
        LOGW("onResume");
    }
}