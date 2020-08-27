//
// Created by Administrator on 2020/8/17 0017.
//

#include "bgfx_wrapper.h"
#include "string.h"

#include <syscall.h>
#include <unistd.h>

using namespace bgfx;

BGFX_FUNC_ENUM(render, bgfx::RendererType::Enum){
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

BGFX_FUNC_NAME(render, bgfx::RendererType::Enum){
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

BGFX_FUNC_INT(clear){
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

BGFX_FUNC_INT(debug){
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

BGFX_FUNC_INT(reset){
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

BGFX_FUNC_ENUM(textureFormat, bgfx::TextureFormat::Enum){
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
BGFX_FUNC_NAME(textureFormat, bgfx::TextureFormat::Enum){
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

/**
 * 	Position,  //!< a_position
			Normal,    //!< a_normal
			Tangent,   //!< a_tangent
			Bitangent, //!< a_bitangent
			Color0,    //!< a_color0
			Color1,    //!< a_color1
			Color2,    //!< a_color2
			Color3,    //!< a_color3
			Indices,   //!< a_indices
			Weight,    //!< a_weight
			TexCoord0, //!< a_texcoord0
			TexCoord1, //!< a_texcoord1
			TexCoord2, //!< a_texcoord2
			TexCoord3, //!< a_texcoord3
			TexCoord4, //!< a_texcoord4
			TexCoord5, //!< a_texcoord5
			TexCoord6, //!< a_texcoord6
			TexCoord7, //!< a_texcoord7

			Count
 */
BGFX_FUNC_ENUM(attrib, bgfx::Attrib::Enum){
    bgfx::Attrib::Enum id;
#define ATTRIB_ID(x)  \
if (strcmp(x, "Position") == 0) id = bgfx::Attrib::Enum::Position; \
else if (strcmp(x, "Normal") == 0) id = bgfx::Attrib::Enum::Normal; \
else if (strcmp(x, "Tangent") == 0) id = bgfx::Attrib::Enum::Tangent; \
else if (strcmp(x, "Bitangent") == 0) id = bgfx::Attrib::Enum::Bitangent; \
else if (strcmp(x, "Color0") == 0) id = bgfx::Attrib::Enum::Color0; \
else if (strcmp(x, "Color1") == 0) id = bgfx::Attrib::Enum::Color1; \
else if (strcmp(x, "Color2") == 0) id = bgfx::Attrib::Enum::Color2; \
else if (strcmp(x, "Color3") == 0) id = bgfx::Attrib::Enum::Color3; \
else if (strcmp(x, "Indices") == 0) id = bgfx::Attrib::Enum::Indices; \
else if (strcmp(x, "Weight") == 0) id = bgfx::Attrib::Enum::Weight; \
else if (strcmp(x, "Indices") == 0) id = bgfx::Attrib::Enum::Indices; \
else if (strcmp(x, "TexCoord0") == 0) id = bgfx::Attrib::Enum::TexCoord0; \
else if (strcmp(x, "TexCoord1") == 0) id = bgfx::Attrib::Enum::TexCoord1; \
else if (strcmp(x, "TexCoord2") == 0) id = bgfx::Attrib::Enum::TexCoord2; \
else if (strcmp(x, "TexCoord3") == 0) id = bgfx::Attrib::Enum::TexCoord3; \
else if (strcmp(x, "TexCoord4") == 0) id = bgfx::Attrib::Enum::TexCoord4; \
else if (strcmp(x, "TexCoord5") == 0) id = bgfx::Attrib::Enum::TexCoord5; \
else if (strcmp(x, "TexCoord6") == 0) id = bgfx::Attrib::Enum::TexCoord6; \
else if (strcmp(x, "TexCoord7") == 0) id = bgfx::Attrib::Enum::TexCoord7; \
else if (strcmp(x, "Count") == 0) id = bgfx::Attrib::Enum::Count; \
else LUA_REPORT_ERROR(L, "attrib", name);
    ATTRIB_ID(name);
    return id;
}
BGFX_FUNC_NAME(attrib, bgfx::Attrib::Enum){
    switch (en) {
        case bgfx::Attrib::Position:
            return "Position";
        case bgfx::Attrib::Normal:
            return "Normal";
        case bgfx::Attrib::Bitangent:
            return "Bitangent";
        case bgfx::Attrib::Color0:
            return "Color0";
        case bgfx::Attrib::Color1:
            return "Color1";
        case bgfx::Attrib::Color2:
            return "Color2";
        case bgfx::Attrib::Color3:
            return "Color3";
        case bgfx::Attrib::Indices:
            return "Indices";
        case bgfx::Attrib::Weight:
            return "Weight";
        case bgfx::Attrib::TexCoord0:
            return "TexCoord0";
        case bgfx::Attrib::TexCoord1:
            return "TexCoord1";
        case bgfx::Attrib::TexCoord2:
            return "TexCoord2";
        case bgfx::Attrib::TexCoord3:
            return "TexCoord3";
        case bgfx::Attrib::TexCoord4:
            return "TexCoord4";
        case bgfx::Attrib::TexCoord5:
            return "TexCoord5";
        case bgfx::Attrib::TexCoord6:
            return "TexCoord6";
        case bgfx::Attrib::TexCoord7:
            return "TexCoord7";
        case bgfx::Attrib::Count:
            return "Count";
    }
    return "unknown_bgfx::Attrib";
}

/**
 *          Uint8,  //!< Uint8
			Uint10, //!< Uint10, availability depends on: `BGFX_CAPS_VERTEX_ATTRIB_UINT10`.
			Int16,  //!< Int16
			Half,   //!< Half, availability depends on: `BGFX_CAPS_VERTEX_ATTRIB_HALF`.
			Float,  //!< Float
			Count
 */
BGFX_FUNC_ENUM(attribType, bgfx::AttribType::Enum){
    bgfx::AttribType::Enum id;
#define AttribType_ID(x)  \
if (strcmp(x, "Uint8") == 0) id = bgfx::AttribType::Enum::Uint8; \
else if (strcmp(x, "Uint10") == 0) id = bgfx::AttribType::Enum::Uint10; \
else if (strcmp(x, "Int16") == 0) id = bgfx::AttribType::Enum::Int16; \
else if (strcmp(x, "Half") == 0) id = bgfx::AttribType::Enum::Half; \
else if (strcmp(x, "Float") == 0) id = bgfx::AttribType::Enum::Float; \
else if (strcmp(x, "Count") == 0) id = bgfx::AttribType::Enum::Count; \
else LUA_REPORT_ERROR(L, "attribType", name);
    AttribType_ID(name);
    return id;
}
BGFX_FUNC_NAME(attribType, bgfx::AttribType::Enum){
    switch (en) {
        case bgfx::AttribType::Uint8:
            return "Uint8";
        case bgfx::AttribType::Uint10:
            return "Uint10";
        case bgfx::AttribType::Int16:
            return "Int16";
        case bgfx::AttribType::Half:
            return "Half";
        case bgfx::AttribType::Float:
            return "Float";
        case bgfx::AttribType::Count:
            return "Count";
    }
    return "unknown_bgfx::AttribType";
}