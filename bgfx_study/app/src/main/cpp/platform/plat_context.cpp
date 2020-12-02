//
// Created by Administrator on 2020/8/2 0002.
//

#ifndef BGFX_STUDY_PLAT_CONTEXT_H
#define BGFX_STUDY_PLAT_CONTEXT_H

#include "plat_context.h"
#include "lua/SkLua.h"
#include "lua/lua.hpp"
#include "lua/int64.h"
#include "nancanvas/Canvas_lua.h"

namespace Platforms{

    static const luaL_Reg bgfx_libs[] = {
            {"bgfx_lua", luaopen_bgfx_lua},
            {"bx_lua", luaopen_bx_lua},
            {"hmem_lua", luaopen_hmem_lua},
            {"canvas_lua", luaopen_canvas_lua},
            {"int64", luaopen_int64},
            {"uint64", luaopen_uint64},
            {nullptr, nullptr}
    };
    extern long long getLuaPtr();

    void initLuaBgfx(){
       // LOGD("_initLuaBgfx");
        lua_State *L = reinterpret_cast<lua_State *>(getLuaPtr());
        luaL_openlibs2(L, bgfx_libs);
    }
};



#endif //BGFX_STUDY_PLAT_CONTEXT_H
