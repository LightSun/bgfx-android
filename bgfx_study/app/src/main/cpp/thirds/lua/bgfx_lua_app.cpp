//
// Created by Administrator on 2020/8/22 0022.
//

#include "bgfx_lua_app.h"
#include "bx/debug.h"
#include "android_pri.h"
//#include <atomic>

namespace Bgfx_lua_app {
    static LuaApp *_app = NULL;
    static entry::InitConfig* _InitConfig = NULL;

    void setInitConfig(entry::InitConfig* config) {
        ext_println("setInitConfig");
        if(_InitConfig != nullptr){
            delete _InitConfig;
        }
        _InitConfig = config;
    }

    LuaApp *newLuaApp(lua_State *L, FUNC_NAME preInit, FUNC_NAME func_init, FUNC_NAME func_draw, FUNC_NAME func_destroy) {
        if (!_app) {
            _app = new LuaApp(L, preInit, func_init, func_draw, func_destroy);
        }
        return _app;
    }

    void destroyLuaApp() {
        if (_app) {
            _app->destroy();
            delete _app;
           _app = NULL;
        }
    }
    void setLuaApp(LuaApp* app){
        if (!_app) {
            _app = app;
        }
    }
}

LuaApp::LuaApp(lua_State *L, FUNC_NAME preInit, FUNC_NAME func_init, FUNC_NAME func_draw, FUNC_NAME func_destroy)
        : L(L), func_preInit(preInit), func_init(func_init), func_draw(func_draw), func_destroy(func_destroy) {
    destroyed = false;
    bgfx_Init = getBgfxInit();
    initConfig = Bgfx_lua_app::_InitConfig;
}

void LuaApp::init() {
    bgfx_Init.platformData.nwh = initConfig->window;
    bgfx_Init.resolution.width = initConfig->win_width;
    bgfx_Init.resolution.height = initConfig->win_height;
    bgfx::init(bgfx_Init);
    if (func_init) {
        lua_getglobal(L, func_init);
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            luaL_error(L, "call LuaApp init failed. func = %s", func_init);
        }
    }
}

void LuaApp::startLoop() {
    if (!m_thread.isRunning()) {
        m_thread.init(LuaApp::threadFunc, this, 0, "bgfx_lua");
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
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            luaL_error(L, "call LuaApp pre-init failed. func = %s", func_preInit);
        }
    }
}

int32_t LuaApp::threadFunc(bx::Thread *_thread, void *_userData) {
    BX_UNUSED(_thread);
    //bgfx init and draw must in one thread.
    LuaApp *demo = static_cast<LuaApp *>(_userData);
    ext_println("---- loop start ------");
    demo->doPreInit();
    demo->init();
    bgfx::frame();
    //int32_t result = chdir("/sdcard/bgfx/examples/runtime");
    // BX_ASSERT(0 == result, "Failed to chdir to dir. android.permission.WRITE_EXTERNAL_STORAGE?", errno);
    ext_println("loop draw >>> start");
    int i = 0;
    while (demo->draw() == 0) {
        //bx::debugPrintf
        bx::debugPrintf("loop draw >>> %d", ++i);
    }
    ext_println("loop draw >>> end");
    return 0;
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
        m_thread.shutdown();
        releaseWindow(getBgfxInit().platformData.nwh);
        bgfx::shutdown();
    }
}

bool LuaApp::isDestroyed() {
    return destroyed;
}