//
// Created by Administrator on 2020/9/10 0010.
//

#ifndef BGFX_STUDY_SKMEMORY_LUA_H
#define BGFX_STUDY_SKMEMORY_LUA_H

#include "LuaUtils.h"

extern "C" int luaopen_hmem_lua(lua_State *L);

class SkMemoryLua{
public:
    static void registers(lua_State *L);
};

#endif //BGFX_STUDY_SKMEMORY_LUA_H
