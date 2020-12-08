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

#include "../../platform/plat_context.h"

using namespace h7;

//#include <memory>
namespace h7 {
#define KEY_APP_HOLDER "$_LuaAppHolder_"

    void requestRender(){
        auto L = reinterpret_cast<lua_State *>(Platforms::getLuaPtr());
        getAppHolder(L)->requestRender();
    }
    void getDisplayInfo(int* out){
        auto L = reinterpret_cast<lua_State *>(Platforms::getLuaPtr());
        auto pInit = requireInit(L);
        out[0] = pInit->resolution.width;
        out[1] = pInit->resolution.height;
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

    bool startApp(long long ptr, entry::InitConfig *pConfig) {
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

LuaApp::~LuaApp() {
    _release();
}
//preinit, init, draw, destroy
LuaApp::LuaApp(lua_State *L) {
    this->L = L;
    ref_destroy = LuaUtils::ref(L);
    ref_draw = LuaUtils::ref(L);
    ref_init = LuaUtils::ref(L);
    ref_preInit = LuaUtils::ref(L);
}

void LuaApp::onInit(){
    if (ref_init != LUA_NOREF) {
        LuaUtils::ref_get(L, ref_init);
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            const char *msg = lua_tostring(L, -1);
            LOGE("%s", msg);
            luaL_error(L, "call LuaApp init failed. func = %d, msg = %s", ref_init, msg);
        }
    }
}

void LuaApp::onDraw() {
    if (ref_draw != LUA_NOREF) {
        LuaUtils::ref_get(L, ref_draw);
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            const char *msg = lua_tostring(L, -1);
            LOGE("%s", msg);
            luaL_error(L, "call LuaApp draw failed. func = %d, msg = %s", ref_draw, msg);
        }
    }
}

void LuaApp::onPreInit() {
    if (ref_preInit != LUA_NOREF) {
        LuaUtils::ref_get(L, ref_preInit);
        //luaB_dumpStack(L);
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            const char *msg = lua_tostring(L, -1);
            LOGE("%s", msg);
            luaL_error(L, "call LuaApp pre-init failed. func = %d, msg = %s", ref_preInit, msg);
        } 
    }
}

void LuaApp::onDestroy(bool lightly) {
    _callLuaDestroy();
    ext_println("onDestroy");
    if(!lightly){
        _release();
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
    if (ref_destroy != LUA_NOREF) {
        LuaUtils::ref_get(L, ref_destroy);
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            const char *msg = lua_tostring(L, -1);
            LOGE("%s", msg);
            luaL_error(L, "call LuaApp destroy failed. func = %d, msg = %s", ref_destroy, msg);
        }
    }
}
void LuaApp::_release() {
    if (ref_preInit != LUA_NOREF) {
        LuaUtils::unref(L, ref_preInit);
        ref_preInit = LUA_NOREF;
    }
    if (ref_init != LUA_NOREF) {
        LuaUtils::unref(L, ref_init);
        ref_init = LUA_NOREF;
    }
    if (ref_draw != LUA_NOREF) {
        LuaUtils::unref(L, ref_draw);
        ref_draw = LUA_NOREF;
    }
    if (ref_destroy != LUA_NOREF) {
        LuaUtils::unref(L, ref_destroy);
        ref_destroy = LUA_NOREF;
    }
}

