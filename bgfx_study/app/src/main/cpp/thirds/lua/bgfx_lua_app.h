//
// Created by Administrator on 2020/8/22 0022.
//

#ifndef BGFX_STUDY_BGFX_LUA_APP_H
#define BGFX_STUDY_BGFX_LUA_APP_H

#include "bgfx_wrapper.hpp"
#include "bgfx/bgfx.h"
#include <bx/bx.h>
#include <bx/thread.h>

typedef const char* FUNC_NAME;
class LuaApp;

namespace Bgfx_lua_app{
    void initPlatformData(const bgfx::PlatformData &_data);
    void initPlatformData(void * nwh);
    LuaApp* newLuaApp(lua_State* L, FUNC_NAME func_init, FUNC_NAME func_draw, FUNC_NAME func_destroy);
    void destroyLuaApp();
}

class LuaApp{

public:
    /**
     * create lua app with init, draw, destroy function.
     * @param L the lua state
     * @param func_init the init function name ,must be global stack
     * @param func_draw the draw function name ,must be global stack
     * @param func_destroy the draw destroy name ,must be global stack
     */
    LuaApp(lua_State* L, FUNC_NAME func_init, FUNC_NAME func_draw, FUNC_NAME func_destroy);

    void init();

    //called in sub-thread.
    int draw();

    void destroy();

    bool isDestroyed();

    void startLoop();

private:
    static int32_t threadFunc(bx::Thread* _thread, void* _userData);

    bool destroyed;
    bx::Thread m_thread;
    lua_State* L;
    FUNC_NAME func_init;
    FUNC_NAME func_draw;
    FUNC_NAME func_destroy;
};


#endif //BGFX_STUDY_BGFX_LUA_APP_H
