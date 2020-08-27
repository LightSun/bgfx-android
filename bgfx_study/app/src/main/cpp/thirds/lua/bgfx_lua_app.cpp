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
static int initAfterCallback(lua_State* L){
    LOGD("lua bgfx_init success >>> initAfterCallback");
    LuaAppHolder *pHolder = Bgfx_lua_app::getAppHolder(L);
    if(!pHolder->app->isQuit()){
        pHolder->sendCmd(new CmdData(TYPE_BGFX_INIT_DONE, (void *)nullptr));
    }
    return 0;
}

static int init_error(lua_State* L){
    const char *msg = lua_tostring(L, -1);
    LOGE("lua init_error: %s", msg);
    return 0;
}

bool LuaApp::init(LuaAppHolder *holder) {
    auto pConfig = holder->config;
    Init *pInit = holder->bgfx_init;
    pInit->platformData.nwh = pConfig->window;
    pInit->resolution.width = pConfig->win_width;
    pInit->resolution.height = pConfig->win_height;
    bgfx::init(*pInit);
    LOGD("bgfx init is called. holder = %p,func_init= %s ", holder, func_init);

    if (func_init) {
        lua_getglobal(L, func_init);
    }
    return true;

    //1: func_error(string)
    //2: func_result(...)
    //3: func() -> ...
    //4: ... : parameters.
    //5: count of parameters
    //6: count of func result
    if (func_init) {
        lua_pushcfunction(L, init_error);
        lua_pushcfunction(L, initAfterCallback);
        lua_getglobal(L, func_init);
        lua_pushnumber(L, 0);
        lua_pushnumber(L, 0);
        lua_runMain(L);
        LOGD("bgfx_runMain is called for func_init. wait callback.");
        /*if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            luaL_error(L, "call LuaApp init failed. func = %s", func_init);
        }*/
        return false;
    } else{
        return true;
    }
}

int LuaApp::draw() {
    if (func_draw) {
        lua_getglobal(L, func_draw);
        if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
            luaL_error(L, "call LuaApp draw failed. func = %s", func_draw);
        } else {
            return TO_NUMBER_16(L, -1);
        }
    }
    return -1;
}

void LuaApp::doPreInit() {
    if (func_preInit) {
        lua_getglobal(L, func_preInit);
        LOGD("doPreInit");
        luaB_dumpStack(L);
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            luaL_error(L, "call LuaApp pre-init failed. func = %s", func_preInit);
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

                case TYPE_BGFX_INIT_DONE:
                    performApp(holder);
                    break;

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