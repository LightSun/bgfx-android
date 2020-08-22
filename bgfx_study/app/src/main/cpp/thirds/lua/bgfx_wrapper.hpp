//
// Created by Administrator on 2020/8/17 0017.
//

#ifndef BGFX_STUDY_BGFX_WRAPPER_HPP
#define BGFX_STUDY_BGFX_WRAPPER_HPP

#include "bgfx/bgfx.h"
#include "bx/debug.h"
#include "../../src/global.h"
#include "lua.hpp"

//log and report
#define LUA_REPORT_ERROR(L, type, x) \
    bx::debugPrintf("Invalid bgfx %s type %s", #type, #x); \
    luaL_error(L, "Invalid bgfx %s type %s", #type, #x);

#define TO_NUMBER_8(L, idx) static_cast<uint8_t>(lua_tonumber(L, idx))
#define TO_NUMBER_16(L, idx) static_cast<uint16_t>(lua_tonumber(L, idx))
#define TO_NUMBER_32(L, idx) static_cast<uint32_t>(lua_tonumber(L, idx))
#define BGFX_FUNC_INT(type) int bgfx_ ##type(lua_State* L,const char* name){
#define BGFX_FUNC_NAME(type, ENUM_T) const char* bgfx_ ##type ##_name(lua_State* L,ENUM_T en){
#define BGFX_FUNC_ENUM(type, ENUM_T) ENUM_T bgfx_ ##type ##_enum(lua_State* L,const char* name){

using namespace bgfx;

BGFX_FUNC_ENUM(render, bgfx::RendererType::Enum)
//bgfx::RendererType::Enum bgfx_render_enum(lua_State* L,const char* name){
    bgfx::RendererType::Enum id = bgfx::RendererType::Enum::Count;
#define RENDERER_TYPE_ID(x)  \
if (strcmp(x, "Count") == 0) id = bgfx::RendererType::Enum::Count; \
else if (strcmp(x, "Noop") == 0) id = bgfx::RendererType::Enum::Noop; \
else if (strcmp(x, "Direct3D9") == 0) id = bgfx::RendererType::Enum::Direct3D9; \
else if (strcmp(x, "Direct3D11") == 0) id = bgfx::RendererType::Enum::Direct3D11; \
else if (strcmp(x, "Direct3D12") == 0) id = bgfx::RendererType::Enum::Direct3D12; \
else if (strcmp(x, "Gnm") == 0) id = bgfx::RendererType::Enum::Gnm; \
else if (strcmp(x, "Metal") == 0) id = bgfx::RendererType::Enum::Metal; \
else if (strcmp(x, "Nvn") == 0) id = bgfx::RendererType::Enum::Nvn; \
else if (strcmp(x, "OpenGLES") == 0) id = bgfx::RendererType::Enum::OpenGLES; \
else if (strcmp(x, "OpenGL") == 0) id = bgfx::RendererType::Enum::OpenGL; \
else if (strcmp(x, "Vulkan") == 0) id = bgfx::RendererType::Enum::Vulkan; \
else if (strcmp(x, "WebGPU") == 0) id = bgfx::RendererType::Enum::WebGPU; \
else LUA_REPORT_ERROR(L, "renderer", name);
    RENDERER_TYPE_ID(name);
    return id;
}

BGFX_FUNC_NAME(render, bgfx::RendererType::Enum)
//const char* bgfx_render_name(lua_State* L,bgfx::RendererType::Enum en){
    switch (en){
        case bgfx::RendererType::Enum::Count:
            return "Count";
        case bgfx::RendererType::Enum::Noop:
            return "Noop";
        case bgfx::RendererType::Enum::Direct3D9:
            return "Direct3D9";
        case bgfx::RendererType::Enum::Direct3D11:
            return "Direct3D11";
        case bgfx::RendererType::Enum::Direct3D12:
            return "Direct3D12";
        case bgfx::RendererType::Enum::Gnm:
            return "Gnm";
        case bgfx::RendererType::Enum::Metal:
            return "Metal";
        case bgfx::RendererType::Enum::Nvn:
            return "Nvn";
        case bgfx::RendererType::Enum::OpenGLES:
            return "OpenGLES";
        case bgfx::RendererType::Enum::OpenGL:
            return "OpenGL";
        case bgfx::RendererType::Enum::Vulkan:
            return "Vulkan";
        case bgfx::RendererType::Enum::WebGPU:
            return "WebGPU";
    }
    LUA_REPORT_ERROR(L, "support render", en);
    return "Count";
}

BGFX_FUNC_INT(clear)
    int id = BGFX_CLEAR_NONE;
#define CLEAR_ID(x) else if (strcmp(name, #x) == 0) id = x
    if(0);
    CLEAR_ID(BGFX_CLEAR_NONE);
    CLEAR_ID(BGFX_CLEAR_COLOR);
    CLEAR_ID(BGFX_CLEAR_DEPTH);
    CLEAR_ID(BGFX_CLEAR_STENCIL);
    CLEAR_ID(BGFX_CLEAR_DISCARD_COLOR_0);
    CLEAR_ID(BGFX_CLEAR_DISCARD_COLOR_1);
    CLEAR_ID(BGFX_CLEAR_DISCARD_COLOR_2);
    CLEAR_ID(BGFX_CLEAR_DISCARD_COLOR_3);
    CLEAR_ID(BGFX_CLEAR_DISCARD_COLOR_4);
    CLEAR_ID(BGFX_CLEAR_DISCARD_COLOR_5);
    CLEAR_ID(BGFX_CLEAR_DISCARD_COLOR_6);
    CLEAR_ID(BGFX_CLEAR_DISCARD_COLOR_7);
    CLEAR_ID(BGFX_CLEAR_DISCARD_DEPTH);
    CLEAR_ID(BGFX_CLEAR_DISCARD_STENCIL);
    CLEAR_ID(BGFX_CLEAR_DISCARD_COLOR_MASK);
    CLEAR_ID(BGFX_CLEAR_DISCARD_MASK);
    else
        LUA_REPORT_ERROR(L, "clear", name);

    return id;
}

BGFX_FUNC_INT(debug)
    int id = BGFX_DEBUG_NONE;
#define DEBUG_ID(x) else if (strcmp(name, #x) == 0) id = x
    if(0);
    DEBUG_ID(BGFX_DEBUG_NONE);
    DEBUG_ID(BGFX_DEBUG_WIREFRAME);
    DEBUG_ID(BGFX_DEBUG_IFH);
    DEBUG_ID(BGFX_DEBUG_STATS);
    DEBUG_ID(BGFX_DEBUG_TEXT);
    DEBUG_ID(BGFX_DEBUG_PROFILER);
    else
        LUA_REPORT_ERROR(L, "debug", name);
    return id;
}

BGFX_FUNC_INT(reset)
    int id = BGFX_RESET_VSYNC;
#define RESET_ID(x) else if (strcmp(name, #x) == 0) id = x
    if(0);
    RESET_ID(BGFX_RESET_NONE);
    RESET_ID(BGFX_RESET_VSYNC);
    RESET_ID(BGFX_RESET_FULLSCREEN);
    RESET_ID(BGFX_RESET_MAXANISOTROPY);
    RESET_ID(BGFX_RESET_CAPTURE);
    RESET_ID(BGFX_RESET_FLUSH_AFTER_RENDER);

    RESET_ID(BGFX_RESET_FLIP_AFTER_RENDER);
    RESET_ID(BGFX_RESET_SRGB_BACKBUFFER);
    RESET_ID(BGFX_RESET_HDR10);
    RESET_ID(BGFX_RESET_HIDPI);
    RESET_ID(BGFX_RESET_DEPTH_CLAMP);
    RESET_ID(BGFX_RESET_SUSPEND);

    RESET_ID(BGFX_RESET_MSAA_X2);
    RESET_ID(BGFX_RESET_MSAA_X4);
    RESET_ID(BGFX_RESET_MSAA_X8);
    RESET_ID(BGFX_RESET_MSAA_X16);
    else
        LUA_REPORT_ERROR(L, "reset", name);
    return id;
}

BGFX_FUNC_ENUM(textureFormat, bgfx::TextureFormat::Enum)
//TODo more enum
    bgfx::TextureFormat::Enum id;
#define TEXTURE_FORMAT_ID(x)  \
if (strcmp(x, "BC1") == 0) id = bgfx::TextureFormat::Enum::BC1; \
else if (strcmp(x, "BC2") == 0) id = bgfx::TextureFormat::Enum::BC2; \
else if (strcmp(x, "BC2") == 0) id = bgfx::TextureFormat::Enum::BC2; \
else if (strcmp(x, "BC3") == 0) id = bgfx::TextureFormat::Enum::BC3; \
else if (strcmp(x, "BC4") == 0) id = bgfx::TextureFormat::Enum::BC4; \
else if (strcmp(x, "BC5") == 0) id = bgfx::TextureFormat::Enum::BC5; \
else if (strcmp(x, "BC6H") == 0) id = bgfx::TextureFormat::Enum::BC6H; \
else if (strcmp(x, "BC7") == 0) id = bgfx::TextureFormat::Enum::BC7; \
else if (strcmp(x, "ETC1") == 0) id = bgfx::TextureFormat::Enum::ETC1; \
else LUA_REPORT_ERROR(L, "textureFormat", name);
    TEXTURE_FORMAT_ID(name);
    return id;
}
BGFX_FUNC_NAME(textureFormat, bgfx::TextureFormat::Enum)
    switch (en){
        case TextureFormat::Enum::BC1:
            return "BC1";
        case TextureFormat::Enum::BC2:
            return "BC2";
        case TextureFormat::Enum::BC3:
            return "BC3";
        case TextureFormat::Enum::BC4:
            return "BC4";
        case TextureFormat::Enum::BC5:
            return "BC5";
        case TextureFormat::Enum::BC6H:
            return "BC6H";
        case TextureFormat::Enum::BC7:
            return "BC7";
        case TextureFormat::Enum::ETC1:
            return "ETC1";
        case TextureFormat::Enum::ETC2:
            return "ETC2";
        case TextureFormat::Enum::ETC2A:
            return "ETC2A";
        case TextureFormat::Enum::ETC2A1:
            return "ETC2A1";
        case TextureFormat::Enum::PTC12:
            return "PTC12";
        case TextureFormat::Enum::PTC14:
            return "PTC14";
        case TextureFormat::Enum::PTC12A:
            return "PTC12A";
        case TextureFormat::Enum::PTC14A:
            return "PTC14A";
        case TextureFormat::Enum::PTC22:
            return "PTC22";
        case TextureFormat::Enum::PTC24:
            return "PTC24";
        case TextureFormat::Enum::ATC:
            return "ATC";
        case TextureFormat::Enum::ATCE:
            return "ATCE";
        case TextureFormat::Enum::ATCI:
            return "ATCI";
        case TextureFormat::Enum::ASTC4x4:
            return "ASTC4x4";
        case TextureFormat::Enum::ASTC5x5:
            return "ASTC5x5";
        case TextureFormat::Enum::ASTC6x6:
            return "ASTC6x6";
        case TextureFormat::Enum::ASTC8x5:
            return "ASTC8x5";
        case TextureFormat::Enum::ASTC8x6:
            return "ASTC8x6";
        case TextureFormat::Enum::ASTC10x5:
            return "ASTC10x5";
        case TextureFormat::Enum::Unknown:
            return "Unknown";
        case TextureFormat::Enum::R1:
            return "R1";
        case TextureFormat::Enum::A8:
            return "A8";
            //TODO etc...
        default:
            return "unknown_TextureFormat";
    }
}

extern bgfx::Init& getBgfxInit();


#endif //BGFX_STUDY_BGFX_WRAPPER_HPP
