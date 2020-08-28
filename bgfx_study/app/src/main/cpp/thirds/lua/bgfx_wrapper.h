//
// Created by Administrator on 2020/8/17 0017.
//

#ifndef BGFX_STUDY_BGFX_WRAPPER_H
#define BGFX_STUDY_BGFX_WRAPPER_H

#include "bgfx/bgfx.h"
#include "bx/debug.h"
#include "bx/math.h"
#include "../../core/global.h"
#include "lua.hpp"

//log and report
#define LUA_REPORT_ERROR(L, type, x) \
    {bx::debugPrintf("Invalid bgfx %s type %s", #type, x); \
    luaL_error(L, "Invalid bgfx %s type %s", #type, x);}

#define BGFX_FUNC_INT(type) int bgfx_ ##type(lua_State* L,const char* name)
#define BGFX_FUNC_NAME(type, ENUM_T) const char* bgfx_ ##type ##_name(lua_State* L,ENUM_T en)
#define BGFX_FUNC_ENUM(type, ENUM_T) ENUM_T bgfx_ ##type ##_enum(lua_State* L,const char* name)

BGFX_FUNC_ENUM(render, bgfx::RendererType::Enum);
BGFX_FUNC_NAME(render, bgfx::RendererType::Enum);

BGFX_FUNC_INT(clear);
BGFX_FUNC_INT(debug);
BGFX_FUNC_INT(reset);

BGFX_FUNC_ENUM(textureFormat, bgfx::TextureFormat::Enum);
BGFX_FUNC_NAME(textureFormat, bgfx::TextureFormat::Enum);

BGFX_FUNC_ENUM(attrib, bgfx::Attrib::Enum);
BGFX_FUNC_NAME(attrib, bgfx::Attrib::Enum);

BGFX_FUNC_ENUM(attribType, bgfx::AttribType::Enum);
BGFX_FUNC_NAME(attribType, bgfx::AttribType::Enum);

BGFX_FUNC_ENUM(handness, bx::Handness::Enum);
BGFX_FUNC_NAME(handness, bx::Handness::Enum);

extern bgfx::Init *getBgfxInit(lua_State *pState);

#endif //BGFX_STUDY_BGFX_WRAPPER_H
