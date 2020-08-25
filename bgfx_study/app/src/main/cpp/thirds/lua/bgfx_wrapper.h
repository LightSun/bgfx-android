//
// Created by Administrator on 2020/8/17 0017.
//

#ifndef BGFX_STUDY_BGFX_WRAPPER_H
#define BGFX_STUDY_BGFX_WRAPPER_H

#include "bgfx/bgfx.h"
#include "bx/debug.h"
#include "../../core/global.h"
#include "lua.hpp"

//log and report
#define LUA_REPORT_ERROR(L, type, x) \
    bx::debugPrintf("Invalid bgfx %s type %s", #type, x); \
    luaL_error(L, "Invalid bgfx %s type %s", #type, x);

#define TO_NUMBER_8(L, idx) static_cast<uint8_t>(lua_tonumber(L, idx))
#define TO_NUMBER_16(L, idx) static_cast<uint16_t>(lua_tonumber(L, idx))
#define TO_NUMBER_32(L, idx) static_cast<uint32_t>(lua_tonumber(L, idx))
#define BGFX_FUNC_INT(type) int bgfx_ ##type(lua_State* L,const char* name)
#define BGFX_FUNC_NAME(type, ENUM_T) const char* bgfx_ ##type ##_name(lua_State* L,ENUM_T en)
#define BGFX_FUNC_ENUM(type, ENUM_T) ENUM_T bgfx_ ##type ##_enum(lua_State* L,const char* name)

using namespace bgfx;

BGFX_FUNC_ENUM(render, bgfx::RendererType::Enum);
BGFX_FUNC_NAME(render, bgfx::RendererType::Enum);

BGFX_FUNC_INT(clear);
BGFX_FUNC_INT(debug);
BGFX_FUNC_INT(reset);

BGFX_FUNC_ENUM(textureFormat, bgfx::TextureFormat::Enum);
BGFX_FUNC_NAME(textureFormat, bgfx::TextureFormat::Enum);

extern Init *getBgfxInit(lua_State *pState);

long getThreadID();

#endif //BGFX_STUDY_BGFX_WRAPPER_H
