//
// Created by Administrator on 2020/8/22 0022.
//

#include "LuaUtils.h"
#include "bgfx_lua_app.h"
#include "bx/debug.h"
#include "android_pri.h"
#include <atomic>

//#include <memory>
namespace Bgfx_lua_app {
#define KEY_APP_HOLDER "$_LuaAppHolder_"

    static inline void androidSetWindow(::ANativeWindow* _window){
        bgfx::PlatformData pd;
        pd.ndt          = NULL;
        pd.nwh          = _window;
        pd.context      = NULL;
        pd.backBuffer   = NULL;
        pd.backBufferDS = NULL;
        bgfx::setPlatformData(pd);
    }

    void releaseWindow(long ptr){
        if(ptr != 0){
            auto L = reinterpret_cast<lua_State *>(ptr);
            Platforms::releaseWindow(getAppHolder(L)->bgfx_init->platformData.nwh);
        }
    }

    void onLifecycle(long ptr, jint mark) {
        if(ptr == 0){
            return;
        }
        auto L = reinterpret_cast<lua_State *>(ptr);
#define ON_PAUSE 3
#define ON_RESUME 2
#define ON_DESTROY 5
        switch (mark){
            case ON_PAUSE: {
                //todo callback lua onPause
                auto pHolder = getAppHolder(L);
                auto app = pHolder->app;
                if(app != NULL){
                    pHolder->pause();
                }
            }
                break;
            case ON_RESUME:{
                auto pHolder = getAppHolder(L);
                if(pHolder->app != NULL){
                    pHolder->resume();
                }
            }
                break;
            case ON_DESTROY:
                getAppHolder(L)->quitAll();
                break;
        }
    }

    bool startApp(long ptr, entry::InitConfig *pConfig) {
        if(ptr == 0){
            return false;
        }
        lua_State* L = reinterpret_cast<lua_State *>(ptr);
        auto pHolder = getAppHolder(L);
        if(pHolder->isRunning()){
            delete pHolder->config;
            pHolder->config = pConfig;
            LOGD("startApp >>> reset InitConfig >>> ");
        } else{
            pHolder->startLoop(pConfig);
        }
        return true;
    }

    LuaAppHolder* getAppHolder(lua_State* L) {
        LuaAppHolder* result;
        lua_getglobal(L, KEY_APP_HOLDER);
        if(lua_isnil(L, -1)){
            lua_pop(L, 1);
            LOGD("create getAppHolder---- ");
            //luaB_dumpStack(L);
            LuaAppHolder** p = (LuaAppHolder**)lua_newuserdata(L, sizeof(LuaAppHolder*));
            *p = result = new LuaAppHolder();

            luaL_getmetatable(L, "ud_LuaAppHolder_");
            lua_setmetatable(L, -2);
            lua_setglobal(L, KEY_APP_HOLDER);
            //luaB_dumpStack(L);
            LOGD("end create getAppHolder---- ");
        } else{
            LuaAppHolder** p = static_cast<LuaAppHolder **>(lua_touserdata(L, -1));
            lua_pop(L, 1);
            result = *p;
        }
        return result;
    }

    bgfx::Init *requireInit(lua_State* L) {
        auto pHolder = getAppHolder(L);
        LOGD("app-holder = %p", pHolder);
        if (!pHolder->bgfx_init) {
            pHolder->bgfx_init = new bgfx::Init();
            LOGD("requireInit >>> bgfx_init: addr = %p", pHolder->bgfx_init);
        }
        return pHolder->bgfx_init;
    }
}

void LuaAppHolder::destroyApp() {
    if (app) {
        app->actDestroy();
        delete app;
        app = nullptr;
    }
}
void LuaAppHolder::quitApp() {
    if (app) {
        app->quit();
    }
}

LuaAppHolder::LuaAppHolder() {
    bgfx_init = nullptr;
    config = nullptr;
    app = nullptr;
}

LuaAppHolder::~LuaAppHolder() {
    DESTROY_POINTER(config);
    DESTROY_POINTER(bgfx_init);
    destroyApp();
}

void LuaAppHolder::startLoop(entry::InitConfig *pConfig) {
    LOGD("startLoop: InitConfig = %p", pConfig);
    config = pConfig;
    if (!m_thread.isRunning()) {
        m_thread.init(LuaAppHolder::threadFunc, this, 0, "bgfx_lua");
    }
}

//-----------------------------------------------------------------------------------------

LuaApp::LuaApp(lua_State *L, FUNC_NAME preInit, FUNC_NAME func_init, FUNC_NAME func_draw,
               FUNC_NAME func_destroy) {
    this->L = L;
    this->func_preInit = preInit;
    this->func_init = func_init;
    this->func_draw = func_draw;
    this->func_destroy = func_destroy;
}

void LuaApp::init(LuaAppHolder *holder) {
    auto pConfig = holder->config;
    bgfx::Init *pInit = holder->bgfx_init;
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
    LOGD("bgfx init is called. holder = %p,func_init= %s ", holder, func_init);

    if (func_init) {
        lua_getglobal(L, func_init);
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            const char *msg = lua_tostring(L, -1);
            LOGE("%s", msg);
            luaL_error(L, "call LuaApp init failed. func = %s, msg = %s", func_init, msg);
        }
    }
}

void LuaApp::draw() {
    if (func_draw) {
        lua_getglobal(L, func_draw);
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            const char *msg = lua_tostring(L, -1);
            LOGE("%s", msg);
            luaL_error(L, "call LuaApp draw failed. func = %s, msg = %s", func_draw, msg);
        }
    }
}

void LuaApp::doPreInit() {
    if (func_preInit) {
        lua_getglobal(L, func_preInit);
        //LOGD("doPreInit");
        //luaB_dumpStack(L);
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            const char *msg = lua_tostring(L, -1);
            LOGE("%s", msg);
            luaL_error(L, "call LuaApp pre-init failed. func = %s, msg = %s", func_preInit, msg);
        } 
    }
}

void LuaApp::actDestroy(bool lightly) {
    if(lightly){
        _callLuaDestroy();
        bgfx::shutdown();
    } else{
        if (getState() != APP_STATE_DESTROYED) {
            setState(APP_STATE_DESTROYED);
            ext_println("destroy");
            //call destroy for lua
            _callLuaDestroy();

            lua_pushnil(L);
            lua_setglobal(L, func_preInit);

            lua_pushnil(L);
            lua_setglobal(L, func_init);

            lua_pushnil(L);
            lua_setglobal(L, func_draw);

            lua_pushnil(L);
            lua_setglobal(L, func_destroy);
            //destroy thread. window , and bgfx
            //m_thread.shutdown();
            bgfx::shutdown();
        }
    }
}

bool LuaApp::isDestroyed() {
    return getState() == APP_STATE_DESTROYED;
}

void LuaApp::quit() {
    LOGD("LuaApp quit:...");
    setState(APP_STATE_TO_QUIT);
}

bool LuaApp::isRunning() {
    return state.load(std::memory_order_relaxed) == APP_STATE_RUNNING;
}
void LuaApp::markRunning() {
    setState(APP_STATE_RUNNING);
}
unsigned char LuaApp::getState() {
    return state.load(std::memory_order_relaxed);
}
void LuaApp::pause() {
    setState(APP_STATE_PAUSED);
}
void LuaApp::setState(unsigned char s) {
    state.store(s, std::memory_order_release);//对读线程可见
}
bool LuaApp::isPaused() {
    return getState() == APP_STATE_PAUSED;
}

void LuaApp::onPause() {
    //TODO perform pause to lua
    LOGW("onPause");
}

void LuaApp::onResume() {
    //TODO perform resume to lua
    LOGW("onResume");
}
void LuaApp::_callLuaDestroy() {
    if (func_destroy) {
        lua_getglobal(L, func_destroy);
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            const char *msg = lua_tostring(L, -1);
            LOGE("%s", msg);
            luaL_error(L, "call LuaApp destroy failed. func = %s, msg = %s", func_destroy, msg);
        }
    }
}
//---------------------------------------------------------------------

static inline void performApp(LuaAppHolder *holder){
    LuaApp *demo = holder->app;
    bgfx::frame();
    LOGD("loop draw >>> start. app = %p", demo);
    for(;; ){
        switch (demo->getState()){
            case APP_STATE_PAUSED:
                LOGD("APP_STATE_PAUSED");
                goto ndes;

            case APP_STATE_RUNNING:
                LOGD("APP_STATE_RUNNING");
                demo->draw();
                //bx::debugPrintf("loop draw >>> %d", ++i);
                break;
            case APP_STATE_NONE:
                LOGD("APP_STATE_NONE");
                break;

            case APP_STATE_DESTROYED:
                LOGD("APP_STATE_DESTROYED");
                goto out;

            case APP_STATE_TO_QUIT:
                LOGD("APP_STATE_NONE");
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

int32_t LuaAppHolder::threadFunc(bx::Thread *_thread, void *_userData) {
    //BX_UNUSED(_thread);
    LOGD("threadFunc: start.");
    LuaAppHolder *holder = static_cast<LuaAppHolder *>(_userData);
    CmdData *data = nullptr;
    bool shouldBreak = false;

    //bgfx api: should call in one thread.
    while (true) {
        auto pVoid = _thread->pop();
        if (pVoid != nullptr) {
            data = static_cast<CmdData *>(pVoid);
            switch (data->type) {
                case TYPE_LUA_APP_INIT: {
                    LuaApp *demo = holder->app = static_cast<LuaApp *>(data->data);
                    LOGD("TYPE_LUA_APP_INIT , start LuaApp : %p", demo);
                    demo->markRunning();
                    demo->doPreInit();
                    demo->init(holder);
                    performApp(holder);
                    break;
                }

                case TYPE_LUA_APP_PAUSE:{
                    LOGD("TYPE_LUA_APP_PAUSE...");
                    std::unique_lock<std::mutex> lock(holder->_mutex);
                    holder->_condition.wait(lock);
                    auto pApp = holder->app;
                    if(pApp != nullptr && !pApp->isDestroyed()){
                        //as bgfx should destroy onPause, we should reinit here
                        pApp->markRunning();
                        pApp->doPreInit();
                        pApp->init(holder);
                        pApp->onResume();
                        //restart loop for draw
                        LOGD("resume: restart for draw.");
                        performApp(holder);
                    }
                }
                    break;

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

void LuaAppHolder::quitAll(EndTask task) {
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

void LuaAppHolder::start(LuaApp *app) {
    if(this->app != nullptr){
        LOGW("start LuaApp failed: last app exists,ptr = %p. you should quit first", this->app);
        return;
    }
    m_thread.push(new CmdData(TYPE_LUA_APP_INIT, app));
    LOGD("_appInit init ok. try TYPE_LUA_APP_START");
}
void LuaAppHolder::pause(){
    LOGD("LuaAppHolder >>> start pause...");
    if(app->isRunning()){
        m_thread.push(new CmdData(TYPE_LUA_APP_PAUSE, (void*)NULL));
        app->pause();
    }
}
void LuaAppHolder::resume() {
    LOGD("LuaAppHolder >>> start resume...");
    _condition.notify_one();
}

bool LuaAppHolder::isRunning() {
    return app != nullptr && m_thread.isRunning();
}

CmdData::CmdData(uint8_t type, void *data) : type(type), data(data) {

}

CmdData::CmdData(uint8_t type, EndTask task) : type(type), task(task) {

}