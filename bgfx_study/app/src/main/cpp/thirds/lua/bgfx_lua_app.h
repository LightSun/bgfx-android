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
#include <mutex>

#define TYPE_NONE 0;
#define TYPE_LUA_APP_INIT    1
#define TYPE_LUA_APP_PAUSE   2
#define TYPE_QUIT_ALL        10

#define APP_STATE_NONE    1
#define APP_STATE_RUNNING 2
#define APP_STATE_PAUSED  3
#define APP_STATE_TO_QUIT 4
#define APP_STATE_DESTROYED 5

class LuaApp;
class LuaAppHolder;
class CmdData;

typedef const char* FUNC_NAME;
typedef void (*EndTask)(LuaAppHolder * holder);

//防止指令重排. linux 内核可用 cpu_relax函数（效果相同）
//#define barrier() __asm__ __volatile__("": : :"memory")

/*#define lua_runMain(L) \
    LuaAppHolder *pHolder = Bgfx_lua_app::getAppHolder(L); \
    pHolder->config->RunMain(reinterpret_cast<long>(L));*/


namespace Bgfx_lua_app{

    bool startApp(long ptr, entry::InitConfig *pConfig);
    bgfx::Init* requireInit(lua_State* L);
    LuaAppHolder* getAppHolder(lua_State* L);

    void onLifecycle(long luaPtr, jint mark);
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
    void quitAll(EndTask task = NULL);
    void start(LuaApp * app);

    void pause();
    void resume();

    bool isRunning();

private:
    static int32_t threadFunc(bx::Thread* _thread, void* _userData);
    //bx thread can't be static
    bx::Thread m_thread;
    //can't use static
    std::mutex _mutex;
    std::condition_variable _condition;
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
    void init(LuaAppHolder *pConfig);

    //called in sub-thread.
    void draw();

    /** real destroy. not change state */
    void actDestroy();

    bool isDestroyed();

    void quit();

    void doPreInit();

    void resume();

    bool isRunning();

    void pause();

    inline unsigned char getState();

    inline void setState(unsigned char s);
    /**
    * create lua app with init, draw, destroy function.
    * @param L the lua state
    * @param preInit the pre init function name ,must be global stack
    * @param func_init the init function name ,must be global stack
    * @param func_draw the draw function name ,must be global stack
    * @param func_destroy the draw destroy name ,must be global stack
    */
    LuaApp(lua_State* L, FUNC_NAME preInit, FUNC_NAME func_init, FUNC_NAME func_draw, FUNC_NAME func_destroy);

    bool isPaused();

private:
    lua_State* L;
    FUNC_NAME func_preInit;
    FUNC_NAME func_init;
    FUNC_NAME func_draw;
    FUNC_NAME func_destroy;

    std::atomic<uint8_t> state{APP_STATE_NONE};
};


#endif //BGFX_STUDY_BGFX_LUA_APP_H
