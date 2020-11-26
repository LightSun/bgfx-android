//
// Created by Administrator on 2020/11/24 0024.
//

#include <mutex>

#include "bgfx/bgfx.h"
#include "bgfx_wrapper.h"
#include "bgfx_app.h"

#include <log.h>
#include "../core/common.h"
#include "../input/GestureContext.h"

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
//------------- controller =======================
    void AppController::destroyApp() {
        if (app) {
            app->actDestroy();
            delete app;
            app = nullptr;
        }
    }
    void AppController::quitApp() {
        if (app) {
            app->quit();
        }
    }

    AppController::AppController() {
        bgfx_init = nullptr;
        config = nullptr;
        app = nullptr;
    }

    AppController::~AppController() {
        DESTROY_POINTER(config);
        DESTROY_POINTER(bgfx_init);
        destroyApp();
    }

    void AppController::startLoop(entry::InitConfig *pConfig) {
        LOGD("startLoop: InitConfig = %p", pConfig);
        config = pConfig;
        if (!m_thread.isRunning()) {
            m_thread.init(AppController::threadFunc, this, 0, "bgfx_app");
        }
    }

    void AppController::quitAll(EndTask task) {
        LOGD("LuaAppHolder >>> start quit all.");
        m_thread.push(new CmdData(TYPE_QUIT_ALL, task));
        if(app != nullptr){
            //if previous is paused .just resume to destroy
            if(app->isPaused()){
                app->quit();
                resume();
            } else{
                app->quit();
            }
        }
    }

    void AppController::start(BgfxApp *app) {
        if(this->app != nullptr){
            LOGW("start LuaApp failed: last app exists,ptr = %p. you should quit first", this->app);
            return;
        }
        m_thread.push(new CmdData(TYPE_APP_INIT, app));
        LOGD("_appInit init ok. try TYPE_LUA_APP_START");
    }
    void AppController::pause(bool manul){
        LOGD("LuaAppHolder >>> start pause... %d = ",manul);
        if(app!= nullptr && app->isRunning()){
            m_thread.push(new CmdData(manul ? TYPE_APP_PAUSE_MANULLY : TYPE_APP_PAUSE, (void*)NULL));
            app->pause();
        }
    }
    void AppController::resume() {
        LOGD("LuaAppHolder >>> start resume...");
        _condition.notify_one();
    }

    bool AppController::isRunning() {
        return app != nullptr && m_thread.isRunning();
    }
    void AppController::requestRender() {
        m_thread.push(new CmdData(TYPE_APP_RENDER, app));
        LOGD("AppController >>> requestRender");
    }

    CmdData::CmdData(uint8_t type, void *data) : type(type), data(data) {

    }
    CmdData::CmdData(uint8_t type, EndTask task) : type(type), task(task) {

    }

    //---------------------------------------------------------------------

    static inline void renderSelf(AppController *holder){
        BgfxApp *demo = holder->app;
        bgfx::frame();
        LOGD("loop render >>> start. app = %p", demo);
        for(;; ){
            switch (demo->getState()){
                case APP_STATE_PAUSED:
                    LOGD("renderSelf >>> APP_STATE_PAUSED");
                    goto ndes;

                case APP_STATE_RUNNING:
                    LOGD("renderSelf >>> APP_STATE_RUNNING");
                    demo->onDraw();
                    //bx::debugPrintf("loop draw >>> %d", ++i);
                    break;
                case APP_STATE_NONE:
                    LOGD("renderSelf >>> APP_STATE_NONE");
                    break;

                case APP_STATE_DESTROYED:
                    LOGD("renderSelf >>> APP_STATE_DESTROYED");
                    goto out;

                case APP_STATE_TO_QUIT:
                    LOGD("renderSelf >>> APP_STATE_NONE");
                    goto out;

                default:
                    LOGW("can't reach here. wrong state = %d", demo->getState());
                    goto out;
            }
        }
        ndes:    //as bgfx should destroy onPause
        demo->onPause();
        demo->actDestroy(true);
        return;
        out:
        holder->destroyApp();
        LOGD("performApp >>> end");
    }

    static inline void render(AppController *holder){
        BgfxApp *demo = holder->app;
        LOGD("loop render >>> start. app = %p", demo);
        switch (demo->getState()){
            case APP_STATE_PAUSED:
                LOGD("render >>> APP_STATE_PAUSED");
                return;

            case APP_STATE_RUNNING:
                LOGD("render >>> APP_STATE_RUNNING");
                bgfx::frame();
                demo->onDraw();
                //bx::debugPrintf("loop draw >>> %d", ++i);
                break;
            case APP_STATE_NONE:
                LOGD("render >>> APP_STATE_NONE");
                break;

            case APP_STATE_DESTROYED:
                LOGD("render >>> APP_STATE_DESTROYED");
                goto out;

            case APP_STATE_TO_QUIT:
                LOGD("render >>> APP_STATE_NONE");
                goto out;

            default:
                LOGW("can't reach here. wrong state = %d", demo->getState());
                goto out;
        }
        return;
        out:
        holder->destroyApp();
        LOGD("render >>> end");
    }

    int32_t AppController::threadFunc(bx::Thread *_thread, void *_userData) {
        //BX_UNUSED(_thread);
        LOGD("threadFunc: start.");
        AppController *holder = static_cast<AppController *>(_userData);
        CmdData *data = nullptr;
        bool shouldBreak = false;

        //bgfx api: should call in one thread.
        while (true) {
            auto pVoid = _thread->pop();
            if (pVoid != nullptr) {
                data = static_cast<CmdData *>(pVoid);
                switch (data->type) {
                    case TYPE_APP_INIT: {
                        BgfxApp *demo = holder->app = static_cast<BgfxApp *>(data->data);
                        LOGD("TYPE_LUA_APP_INIT , start LuaApp : %p", demo);
                        if(demo != nullptr && !demo->isDestroyed()){
                            demo->markRunning();
                            demo->doInit(holder->bgfx_init, holder->config);
                            demo->onResume();
                            if(demo->renderSelf){
                                renderSelf(holder);
                            } else{
                                render(holder);
                            }
                        }
                        break;
                    }

                    case TYPE_APP_PAUSE:{
                        LOGD("TYPE_LUA_APP_PAUSE...");
                        std::unique_lock<std::mutex> lock(holder->_mutex);
                        holder->_condition.wait(lock);
                        auto pApp = holder->app;
                        if(pApp != nullptr && !pApp->isDestroyed()){
                            //as bgfx should destroy onPause, we should reinit here
                            pApp->markRunning();
                            pApp->doInit(holder->bgfx_init, holder->config);
                            pApp->onResume();
                            //restart loop for draw
                            LOGD("resume: restart for draw.");
                            if(pApp->renderSelf){
                                renderSelf(holder);
                            } else{
                                render(holder);
                            }
                        }
                    }
                        break;

                    case TYPE_APP_PAUSE_MANULLY:{
                        LOGD("TYPE_LUA_APP_PAUSE_MANULLY...");
                        std::unique_lock<std::mutex> lock(holder->_mutex);
                        holder->_condition.wait(lock);
                        auto pApp = holder->app;
                        if(pApp != nullptr && !pApp->isDestroyed()){
                            pApp->markRunning();
                            pApp->onResume();
                            //restart loop for draw
                            LOGD("resume: restart for draw.");
                            if(pApp->renderSelf){
                                renderSelf(holder);
                            } else{
                                render(holder);
                            }
                        }
                    }break;

                    case TYPE_APP_RENDER:{
                        auto pApp = holder->app;
                        if(pApp != nullptr && !pApp->isDestroyed()){
                            render(holder);
                        }
                    }break;

                    case TYPE_QUIT_ALL:
                        LOGD("TYPE_QUIT_ALL.");
                        if (data->task) {
                            data->task(holder);
                        }
                        shouldBreak = true;
                        break;
                }
                //quit all
                delete data;
                if (shouldBreak) {
                    break;
                }
            }
        }
        if(holder->config->OnExitRenderThread){
            holder->config->OnExitRenderThread();
        }
        LOGD("threadFunc: exit.");
        return 0;
    }
}