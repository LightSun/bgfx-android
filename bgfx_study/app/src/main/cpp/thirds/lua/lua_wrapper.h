//
// Created by Administrator on 2020/8/28 0028.
//

#ifndef BGFX_STUDY_LUA_WRAPPER_H
#define BGFX_STUDY_LUA_WRAPPER_H

#include <bx/math.h>
#include "lua.hpp"
#include "bgfx/bgfx.h"
#include "bx/bx.h"
#include "SkMemory.h"
#include "bgfx_lua_app.h"

#ifdef __cplusplus
extern "C"{
#endif

#include <malloc.h>

#ifdef __cplusplus
}
#endif

#define lua2bool(L, idx) lua_toboolean(L, idx) == 1

#define TO_NUMBER_8(L, idx) static_cast<uint8_t>(lua_tonumber(L, idx))
#define TO_NUMBER_16(L, idx) static_cast<uint16_t>(lua_tonumber(L, idx))
#define TO_NUMBER_32(L, idx) static_cast<uint32_t>(lua_tonumber(L, idx))
#define TO_FLOAT(L, idx) static_cast<float>(lua_tonumber(L, idx))

//return the metatable name for a given class
template <typename T> const char* get_mtname();
#define DEF_MTNAME(T)                           \
    template <> const char* get_mtname<T>() {   \
        return #T "_LuaMetaTableName";          \
    }

#define REG_CLASS(L, C)                             \
    do {                                            \
        luaL_newmetatable(L, get_mtname<C>());      \
        lua_pushvalue(L, -1);                       \
        lua_setfield(L, -2, "__index");             \
        luaL_setfuncs(L, g##C##_Methods, 0);        \
        lua_pop(L, 1); /* pop off the meta-table */ \
    } while (0)

//register empty class . just used for 'get_mtname'
#define REG_EMPTY_CLASS(L, C)                       \
    do {                                            \
        luaL_newmetatable(L, get_mtname<C>());      \
        lua_pushvalue(L, -1);                       \
        lua_setfield(L, -2, "__index");             \
        lua_pop(L, 1); /* pop off the meta-table */ \
    } while (0)

//push string and then release
#define PUSH_STRING_PTR(L, ptr) \
const char* str = ptr->toString(); \
lua_pushstring(L, str);\
free((void*)str);

#endif //BGFX_STUDY_LUA_WRAPPER_H
