//
// Created by Administrator on 2020/8/22 0022.
//

#ifndef BGFX_STUDY_BGFX_LUA_APP_H
#define BGFX_STUDY_BGFX_LUA_APP_H

#include "bgfx_wrapper.h"
#include "bgfx/bgfx.h"
#include <bx/bx.h>
#include <bx/thread.h>

typedef const char* FUNC_NAME;
class LuaApp;

namespace Bgfx_lua_app{
    void initPlatformData(void * nwh, int width, int height);
    LuaApp* newLuaApp(lua_State* L, FUNC_NAME preInit, FUNC_NAME func_init, FUNC_NAME func_draw, FUNC_NAME func_destroy);
    void destroyLuaApp();
}

class LuaApp{

public:
    /**
     * create lua app with init, draw, destroy function.
     * @param L the lua state
     * @param preInit the pre init function name ,must be global stack
     * @param func_init the init function name ,must be global stack
     * @param func_draw the draw function name ,must be global stack
     * @param func_destroy the draw destroy name ,must be global stack
     */
    LuaApp(lua_State* L, FUNC_NAME preInit, FUNC_NAME func_init, FUNC_NAME func_draw, FUNC_NAME func_destroy);

    void init();

    //called in sub-thread.
    int draw();

    void destroy();

    bool isDestroyed();

    void startLoop();

private:
    static int32_t threadFunc(bx::Thread* _thread, void* _userData);
    void doPreInit();

    bool destroyed;
    bx::Thread m_thread;
    lua_State* L;
    FUNC_NAME func_preInit;
    FUNC_NAME func_init;
    FUNC_NAME func_draw;
    FUNC_NAME func_destroy;
};


#endif //BGFX_STUDY_BGFX_LUA_APP_H
