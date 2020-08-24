//
// Created by Administrator on 2020/8/22 0022.
//

#ifndef BGFX_STUDY_BGFX_LUA_APP_H
#define BGFX_STUDY_BGFX_LUA_APP_H

#include "bgfx_wrapper.h"
#include "bgfx/bgfx.h"
#include <bx/bx.h>
#include <bx/thread.h>
#include "../core/common.h"

#include <atomic>
#include <stdatomic.h>

typedef const char* FUNC_NAME;
typedef void (*EndTask)();
class LuaApp;
class LuaAppHolder;

namespace Bgfx_lua_app{

    void setInitConfig(entry::InitConfig* config);
    bgfx::Init* requireInit();
    void destroyLuaApp();

    LuaAppHolder& getAppHolder();
}

class LuaAppHolder{
public:
    LuaApp* app;
    bgfx::Init* bgfx_init;
    entry::InitConfig* config;

    ~LuaAppHolder();
    LuaAppHolder();

    void destroyApp();

   /* LuaAppHolder& operator= (LuaAppHolder const& o) noexcept {
        app = o.app;
        config = o.config;
        bgfx_init = o.bgfx_init;
        return *this;
    }*/

    void startLoop(entry::InitConfig *pConfig);
    void quitAll(EndTask task);
    void start(LuaApp * app);

private:
    static int32_t threadFunc(bx::Thread* _thread, void* _userData);
    //bx thread can't be static
    bx::Thread m_thread;
};

#define TYPE_NONE 0;
#define TYPE_LUA_APP_START      1
//#define TYPE_LUA_APP_QUIT      2
#define TYPE_BGFX_INIT    3
#define TYPE_QUIT_ALL     4
class CmdData{
public:
    void * data;
    uint8_t type = TYPE_NONE;
    EndTask task;

    CmdData(uint8_t type, void * data);
    CmdData(uint8_t type, EndTask task);
};

class LuaApp{

public:
    void init(LuaAppHolder *pConfig);

    //called in sub-thread.
    int draw();

    void destroy();

    bool isDestroyed();

    void quit();

    bool isQuit();

    void doPreInit();

    void start();
    /**
    * create lua app with init, draw, destroy function.
    * @param L the lua state
    * @param preInit the pre init function name ,must be global stack
    * @param func_init the init function name ,must be global stack
    * @param func_draw the draw function name ,must be global stack
    * @param func_destroy the draw destroy name ,must be global stack
    */
    LuaApp(lua_State* L, FUNC_NAME preInit, FUNC_NAME func_init, FUNC_NAME func_draw, FUNC_NAME func_destroy);

private:
    bool destroyed;
    lua_State* L;
    FUNC_NAME func_preInit;
    FUNC_NAME func_init;
    FUNC_NAME func_draw;
    FUNC_NAME func_destroy;

    std::atomic<bool> shouldQuit{false};
};


#endif //BGFX_STUDY_BGFX_LUA_APP_H
