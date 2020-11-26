//
// Created by Administrator on 2020/8/22 0022.
//

#include <mutex>
#include <atomic>
#include <stdatomic.h>

#include "bgfx_lua_app.h"
//#include "bgfx_app.h"
#include "bgfx_wrapper.h"

#include "../core/common.h"

#include "log.h"
#include "LuaUtils.h"
#include "bx/debug.h"
#include "bgfx/bgfx.h"

#include "../platform/android_pri.h"

using namespace h7;

//#include <memory>
namespace h7 {
#define KEY_APP_HOLDER "$_LuaAppHolder_"
    static int _win_width;
    static int _win_height;

    void requestRender(){

    }

    int getDisplayWidth(){
        return _win_width;
    }
    int getDisplayHeight(){
        return _win_height;
    }
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
            _win_width = pConfig->win_width;
            _win_height = pConfig->win_height;
            LOGD("startApp >>> reset InitConfig >>> ");
        } else{
            pHolder->startLoop(pConfig);
        }
        return true;
    }

    AppController* getAppHolder(lua_State* L) {
        AppController* result;
        lua_getglobal(L, KEY_APP_HOLDER);
        if(lua_isnil(L, -1)){
            lua_pop(L, 1);
            LOGD("create getAppHolder---- ");
            //luaB_dumpStack(L);
            AppController** p = (AppController**)lua_newuserdata(L, sizeof(AppController*));
            *p = result = new AppController();

            luaL_getmetatable(L, "ud_LuaAppHolder_");
            lua_setmetatable(L, -2);
            lua_setglobal(L, KEY_APP_HOLDER);
            //luaB_dumpStack(L);
            LOGD("end create getAppHolder---- ");
        } else{
            AppController** p = static_cast<AppController **>(lua_touserdata(L, -1));
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

//-----------------------------------------------------------------------------------------

LuaApp::LuaApp(lua_State *L, FUNC_NAME preInit, FUNC_NAME func_init, FUNC_NAME func_draw,
               FUNC_NAME func_destroy) {
    this->L = L;
    this->func_preInit = preInit;
    this->func_init = func_init;
    this->func_draw = func_draw;
    this->func_destroy = func_destroy;
}

void LuaApp::onInit(){
    if (func_init) {
        lua_getglobal(L, func_init);
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            const char *msg = lua_tostring(L, -1);
            LOGE("%s", msg);
            luaL_error(L, "call LuaApp init failed. func = %s, msg = %s", func_init, msg);
        }
    }
}

void LuaApp::onDraw() {
    if (func_draw) {
        lua_getglobal(L, func_draw);
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            const char *msg = lua_tostring(L, -1);
            LOGE("%s", msg);
            luaL_error(L, "call LuaApp draw failed. func = %s, msg = %s", func_draw, msg);
        }
    }
}

void LuaApp::onPreInit() {
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

void LuaApp::onDestroy(bool lightly) {
    _callLuaDestroy();
    ext_println("onDestroy");
    if(!lightly){
        lua_pushnil(L);
        lua_setglobal(L, func_preInit);

        lua_pushnil(L);
        lua_setglobal(L, func_init);

        lua_pushnil(L);
        lua_setglobal(L, func_draw);

        lua_pushnil(L);
        lua_setglobal(L, func_destroy);
    }
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

