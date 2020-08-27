//
// Created by Administrator on 2020/8/22 0022.
//

#include "bgfx_lua_app.h"
#include "bx/debug.h"
#include "android_pri.h"
#include <atomic>

#include "pthread.h"

//#include <memory>
namespace Bgfx_lua_app {
#define KEY_APP_HOLDER "$_LuaAppHolder_"

    void startApp(long ptr, entry::InitConfig *pConfig) {
        lua_State* L = reinterpret_cast<lua_State *>(ptr);
        LuaAppHolder** p = (LuaAppHolder**)lua_newuserdata(L, sizeof(LuaAppHolder*));
        *p = new LuaAppHolder();

        luaL_getmetatable(L, "ud_LuaAppHolder_");
        lua_setmetatable(L, -2);
        lua_setglobal(L, KEY_APP_HOLDER);

        LOGD("create LuaAppHolder = %p", *p);
        (*p)->startLoop(pConfig);
    }

    LuaAppHolder* getAppHolder(lua_State* L) {
        //luaB_dumpStack(L);
        lua_getglobal(L, KEY_APP_HOLDER);
        LuaAppHolder** p = static_cast<LuaAppHolder **>(lua_touserdata(L, -1));
        lua_pop(L, 1);
       // luaB_dumpStack(L);
        return *p;
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

bool LuaApp::init(LuaAppHolder *holder) {
    auto pConfig = holder->config;
    Init *pInit = holder->bgfx_init;
    //LOGD("holder = %p, init = %p, resolution = %p", holder, pInit, &pInit->resolution);
    //LOGD("init config, w = %d, h = %d", pConfig->win_width, pConfig->win_height);
    pInit->platformData.nwh = pConfig->window;
    pInit->resolution.width = pConfig->win_width;
    pInit->resolution.height = pConfig->win_height;
    bgfx::init(*pInit);
    LOGD("bgfx init is called. holder = %p,func_init= %s ", holder, func_init);

    if (func_init) {
        lua_getglobal(L, func_init);
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            const char *msg = lua_tostring(L, -1);
            luaL_error(L, "call LuaApp init failed. func = %s, msg = %s", func_init, msg);
        }
    }
    return true;
}

int LuaApp::draw() {
    if (func_draw) {
        lua_getglobal(L, func_draw);
        if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
            const char *msg = lua_tostring(L, -1);
            luaL_error(L, "call LuaApp draw failed. func = %s, msg = %s", func_draw, msg);
        } else {
            return TO_NUMBER_16(L, -1);
        }
    }
    return -1;
}

void LuaApp::doPreInit() {
    if (func_preInit) {
        lua_getglobal(L, func_preInit);
        //LOGD("doPreInit");
        //luaB_dumpStack(L);
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            const char *msg = lua_tostring(L, -1);
            luaL_error(L, "call LuaApp pre-init failed. func = %s, msg = %s", func_preInit, msg);
        }
    }
}

void LuaApp::start() {
    Bgfx_lua_app::getAppHolder(L)->start(this);
}

void LuaApp::destroy() {
    if (!destroyed) {
        ext_println("destroy");
        destroyed = true;
        //destroy lua func
        if (func_destroy) {
            lua_getglobal(L, func_destroy);
            if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
                luaL_error(L, "call LuaApp destroy failed. func = %s", func_destroy);
            }
        }
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
        // releaseWindow(getBgfxInit()->platformData.nwh);
        // bgfx::shutdown();
    }
}

bool LuaApp::isDestroyed() {
    return destroyed;
}

void LuaApp::quit() {
    shouldQuit.store(true, std::memory_order_release);//对读线程可见
}

bool LuaApp::isQuit() {
    return shouldQuit.load(std::memory_order_relaxed);
}

//---------------------------------------------------------------------

static inline void performApp(LuaAppHolder *holder){
    LuaApp *demo = holder->app;
    bgfx::frame();
    LOGD("loop draw >>> start. app = %p", demo);
    int i = 0;
    while (!demo->isQuit() && demo->draw() == 0) {
        //bx::debugPrintf
        bx::debugPrintf("loop draw >>> %d", ++i);
    }
    LOGD("loop draw >>> end");
    holder->destroyApp();
}

int32_t LuaAppHolder::threadFunc(bx::Thread *_thread, void *_userData) {
    //BX_UNUSED(_thread);
    LOGD("threadFunc: start.");
    LuaAppHolder *holder = static_cast<LuaAppHolder *>(_userData);
    CmdData *data = nullptr;
    LuaApp *demo = nullptr;

    //bgfx init and render must be in one thread.
    //bgfx: non-render api must be called in main thread.
    while (true) {
        auto pVoid = _thread->pop();
        if (pVoid != nullptr) {
            data = static_cast<CmdData *>(pVoid);
            switch (data->type) {
                case TYPE_LUA_APP_INIT: {
                    demo = holder->app = static_cast<LuaApp *>(data->data);
                    LOGD("start LuaApp : %p", demo);
                    if (!demo->isQuit()) {
                        demo->doPreInit();
                        if(demo->init(holder)){
                            performApp(holder);
                        }
                    } else{
                        holder->destroyApp();
                    }
                    break;
                }

                case TYPE_QUIT_ALL:
                    LOGD("quit all.");
                    if (data->task) {
                        data->task();
                    }
                    break;
            }
            //quit all
            if (data->type == TYPE_QUIT_ALL) {
                delete data;
                break;
            } else {
                delete data;
            }
        }
    }
    //m_thread.shutdown();
    releaseWindow(holder->bgfx_init->platformData.nwh);
    bgfx::shutdown();
    if(holder->config->OnExitRenderThread){
        holder->config->OnExitRenderThread();
    }
    LOGD("threadFunc: exit.");
    return 0;
}

void LuaAppHolder::quitAll(EndTask task) {
    m_thread.push(new CmdData(TYPE_QUIT_ALL, task));
}

void LuaAppHolder::start(LuaApp *app) {
    m_thread.push(new CmdData(TYPE_LUA_APP_INIT, app));
    LOGD("_appInit init ok. try TYPE_LUA_APP_START");
}
void LuaAppHolder::sendCmd(CmdData *data) {
    m_thread.push(data);
}

CmdData::CmdData(uint8_t type, void *data) : type(type), data(data) {

}

CmdData::CmdData(uint8_t type, EndTask task) : type(type), task(task) {

}