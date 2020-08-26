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

#define TYPE_NONE 0;
#define TYPE_LUA_APP_INIT    1
//now we can draw.
#define TYPE_BGFX_INIT_DONE    3
#define TYPE_QUIT_ALL         10

typedef const char* FUNC_NAME;
typedef void (*EndTask)();
typedef void (*LuaCallback)();

class LuaApp;
class LuaAppHolder;
class CmdData;
//防止指令重排. linux 内核可用 cpu_relax函数（效果相同）
#define barrier() __asm__ __volatile__("": : :"memory")


#define lua_runMain(L) \
    LuaAppHolder *pHolder = Bgfx_lua_app::getAppHolder(L); \
    pHolder->config->RunMain(reinterpret_cast<long>(L));

namespace Bgfx_lua_app{

    void startApp(long ptr, entry::InitConfig *pConfig);
    bgfx::Init* requireInit(lua_State* L);
    LuaAppHolder* getAppHolder(lua_State* L);
}

class LuaAppHolder{
public:
    LuaApp* app;
    bgfx::Init* bgfx_init;
    entry::InitConfig* config;

    ~LuaAppHolder();
    LuaAppHolder();

    void quitApp();
    void destroyApp();

    void startLoop(entry::InitConfig *pConfig);
    void quitAll(EndTask task);
    void start(LuaApp * app);
    void sendCmd(CmdData * data);

private:
    static int32_t threadFunc(bx::Thread* _thread, void* _userData);
    //bx thread can't be static
    bx::Thread m_thread;
};

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
    /**
     * init success means will draw after this.
     * @param pConfig  the config
     * @return  true if init success.
     */
    bool init(LuaAppHolder *pConfig);

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
