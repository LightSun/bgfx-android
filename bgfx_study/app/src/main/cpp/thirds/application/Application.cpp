//
// Created by Administrator on 2020/12/8 0008.
//

#include <lua/bgfx_app.h>
#include <log.h>
#include "common.h"
#include "Application.h"
#include "scene/SceneHelper.h"
#include "nanovg/nanovg.h"
#include "nanovg/nanovg_bgfx.h"

namespace h7{
    Input* getInput(){
        return Application::get()->input;
    }
    void setInput(Input* input){
        Application::get()->input = input;
    }

    void Application::initStage(int x, int y, int w, int h) {
        if(!stage){
            Viewport* port = new Viewport();
            port->x = x;
            port->y = y;
            port->width = w;
            port->height = h;
            nvgCtx = nvgCreate(1, 0);
            stage = new Stage(port, nvgCtx);
        }
    }
    void Application::layout() {
        if(stage){
            stage->layout();
        }
    }
    Application::~Application() {
        if(stage){
            stage->unref();
            stage = nullptr;
        }
        DESTROY_POINTER(nvgCtx);
    }
    Context& Application::getContext() {
        return context;
    }

    void Application::doInit(bgfx::Init *pInit, entry::InitConfig *pConfig) {
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

    void Application::actDestroy(bool lightly) {
        if (lightly) {
            onDestroy(lightly);
            bgfx::shutdown();
        } else {
            if (getState() != APP_STATE_DESTROYED) {
                setState(APP_STATE_DESTROYED);
                LOGD("destroy");
                onDestroy(lightly);
                //destroy thread. window , and bgfx
                //m_thread.shutdown();
                bgfx::shutdown();
            }
        }
    }

    unsigned char Application::getState() {
        return state.load(std::memory_order_relaxed);
    }

    void Application::setState(unsigned char s) {
        state.store(s, std::memory_order_release);//对读线程可见
    }

    void Application::onPause() {
        LOGW("onPause");
    }

    void Application::onResume() {
        LOGW("onResume");
    }
}