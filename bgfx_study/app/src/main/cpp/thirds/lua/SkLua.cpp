/*
 * Copyright 2013 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <new>
#include <bx/hash.h>
#include "SkLua.h"
#include "SkRefCnt.h"
#include "lua.hpp"
#include "bgfx_wrapper.h"
extern "C"{
#include <cstring>
}

using namespace bgfx;

// return the metatable name for a given class
template <typename T> const char* get_mtname();
#define DEF_MTNAME(T)                           \
    template <> const char* get_mtname<T>() {   \
        return #T "_LuaMetaTableName";          \
    }

DEF_MTNAME(Init)
DEF_MTNAME(PlatformData)
DEF_MTNAME(Resolution)
DEF_MTNAME(Init::Limits)

template <typename T, typename... Args> T* push_new(lua_State* L, Args&&... args) {
    T* addr = (T*)lua_newuserdata(L, sizeof(T));
    new (addr) T(std::forward<Args>(args)...);
    luaL_getmetatable(L, get_mtname<T>());
    lua_setmetatable(L, -2);
    return addr;
}

template <typename T> void push_obj(lua_State* L, const T& obj) {
    new (lua_newuserdata(L, sizeof(T))) T(obj);
    luaL_getmetatable(L, get_mtname<T>());
    lua_setmetatable(L, -2);
}

template <typename T> T* push_ptr(lua_State* L, T* ptr) {
    *(T**)lua_newuserdata(L, sizeof(T*)) = ptr;
    luaL_getmetatable(L, get_mtname<T>());
    lua_setmetatable(L, -2);
    return ptr;
}

template <typename T> T* push_ref(lua_State* L, T* ref) {
    *(T**)lua_newuserdata(L, sizeof(T*)) = SkSafeRef(ref);
    luaL_getmetatable(L, get_mtname<T>());
    lua_setmetatable(L, -2);
    return ref;
}

template <typename T> void push_ref(lua_State* L, sk_sp<T> sp) {
    *(T**)lua_newuserdata(L, sizeof(T*)) = sp.release();
    luaL_getmetatable(L, get_mtname<T>());
    lua_setmetatable(L, -2);
}

template <typename T> T* get_ref(lua_State* L, int index) {
    return *(T**)luaL_checkudata(L, index, get_mtname<T>());
}

template <typename T> T* get_obj(lua_State* L, int index) {
    return (T*)luaL_checkudata(L, index, get_mtname<T>());
}

static bool lua2bool(lua_State* L, int index) {
    return !!lua_toboolean(L, index);
}

///////////////////////////////////////////////////////////////////////////////

SkLua::SkLua(const char termCode[]) : fTermCode(termCode), fWeOwnL(true) {
    fL = luaL_newstate();
    luaL_openlibs(fL);
    SkLua::Load(fL);
}

SkLua::SkLua(lua_State* L) : fL(L), fWeOwnL(false) {}

SkLua::~SkLua() {
    if (fWeOwnL) {
        if (fTermCode.getLength() > 0) {
            lua_getglobal(fL, fTermCode.getPtr());
            if (lua_pcall(fL, 0, 0, 0) != LUA_OK) {
                SkDebugf("lua err: %s\n", lua_tostring(fL, -1));
            }
        }
        lua_close(fL);
    }
}

bool SkLua::runCode(const char code[]) {
    int err = luaL_loadstring(fL, code) || lua_pcall(fL, 0, 0, 0);
    if (err) {
        SkDebugf("--- lua failed: %s\n", lua_tostring(fL, -1));
        return false;
    }
    return true;
}

bool SkLua::runCode(const void* code, size_t size) {
    bx::StringView str((const char*)code, size);
    //SkString str((const char*)code, size);
    return this->runCode(str.getPtr());
}

///////////////////////////////////////////////////////////////////////////////

#define CHECK_SETFIELD(key) do if (key) lua_setfield(fL, -2, key); while (0)

static void setfield_bool_if(lua_State* L, const char key[], bool pred) {
    if (pred) {
        lua_pushboolean(L, true);
        lua_setfield(L, -2, key);
    }
}

static void setfield_string(lua_State* L, const char key[], const char value[]) {
    lua_pushstring(L, value);
    lua_setfield(L, -2, key);
}

static void setfield_number(lua_State* L, const char key[], double value) {
    lua_pushnumber(L, value);
    lua_setfield(L, -2, key);
}

static void setfield_boolean(lua_State* L, const char key[], bool value) {
    lua_pushboolean(L, value);
    lua_setfield(L, -2, key);
}

static void setfield_scalar(lua_State* L, const char key[], SkScalar value) {
    setfield_number(L, key, SkScalarToLua(value));
}

static void setfield_function(lua_State* L,
                              const char key[], lua_CFunction value) {
    lua_pushcfunction(L, value);
    lua_setfield(L, -2, key);
}

static int lua2int_def(lua_State* L, int index, int defaultValue) {
    if (lua_isnumber(L, index)) {
        return (int)lua_tonumber(L, index);
    } else {
        return defaultValue;
    }
}

static SkScalar lua2scalar(lua_State* L, int index) {
    SkASSERT(lua_isnumber(L, index));
    return SkLuaToScalar(lua_tonumber(L, index));
}

static SkScalar lua2scalar_def(lua_State* L, int index, SkScalar defaultValue) {
    if (lua_isnumber(L, index)) {
        return SkLuaToScalar(lua_tonumber(L, index));
    } else {
        return defaultValue;
    }
}

static SkScalar getarray_scalar(lua_State* L, int stackIndex, int arrayIndex) {
    SkASSERT(lua_istable(L, stackIndex));
    lua_rawgeti(L, stackIndex, arrayIndex);

    SkScalar value = lua2scalar(L, -1);
    lua_pop(L, 1);
    return value;
}

static void getarray_scalars(lua_State* L, int stackIndex, SkScalar dst[], int count) {
    for (int i = 0; i < count; ++i) {
        dst[i] = getarray_scalar(L, stackIndex, i + 1);
    }
}

static void setarray_number(lua_State* L, int index, double value) {
    lua_pushnumber(L, value);
    lua_rawseti(L, -2, index);
}

static void setarray_scalar(lua_State* L, int index, SkScalar value) {
    setarray_number(L, index, SkScalarToLua(value));
}

static void setarray_string(lua_State* L, int index, const char str[]) {
    lua_pushstring(L, str);
    lua_rawseti(L, -2, index);
}

void SkLua::pushBool(bool value, const char key[]) {
    lua_pushboolean(fL, value);
    CHECK_SETFIELD(key);
}

void SkLua::pushString(const char str[], const char key[]) {
    lua_pushstring(fL, str);
    CHECK_SETFIELD(key);
}

void SkLua::pushString(const char str[], size_t length, const char key[]) {
    //SkString s(str, length);
    bx::StringView s(str, length);
    lua_pushstring(fL, s.getPtr());
    CHECK_SETFIELD(key);
}

void SkLua::pushString(const bx::StringView& str, const char key[]) {
    lua_pushstring(fL, str.getPtr());
    CHECK_SETFIELD(key);
}

/*void SkLua::pushColor(SkColor color, const char key[]) {
    lua_newtable(fL);
    setfield_number(fL, "a", SkColorGetA(color) / 255.0);
    setfield_number(fL, "r", SkColorGetR(color) / 255.0);
    setfield_number(fL, "g", SkColorGetG(color) / 255.0);
    setfield_number(fL, "b", SkColorGetB(color) / 255.0);
    CHECK_SETFIELD(key);
}*/

void SkLua::pushU32(uint32_t value, const char key[]) {
    lua_pushnumber(fL, (double)value);
    CHECK_SETFIELD(key);
}

void SkLua::pushScalar(SkScalar value, const char key[]) {
    lua_pushnumber(fL, SkScalarToLua(value));
    CHECK_SETFIELD(key);
}

void SkLua::pushArrayU16(const uint16_t array[], int count, const char key[]) {
    lua_newtable(fL);
    for (int i = 0; i < count; ++i) {
        // make it base-1 to match lua convention
        setarray_number(fL, i + 1, (double)array[i]);
    }
    CHECK_SETFIELD(key);
}

/*void SkLua::pushArrayPoint(const SkPoint array[], int count, const char key[]) {
    lua_newtable(fL);
    for (int i = 0; i < count; ++i) {
        // make it base-1 to match lua convention
        lua_newtable(fL);
        this->pushScalar(array[i].fX, "x");
        this->pushScalar(array[i].fY, "y");
        lua_rawseti(fL, -2, i + 1);
    }
    CHECK_SETFIELD(key);
}*/

void SkLua::pushArrayScalar(const SkScalar array[], int count, const char key[]) {
    lua_newtable(fL);
    for (int i = 0; i < count; ++i) {
        // make it base-1 to match lua convention
        setarray_scalar(fL, i + 1, array[i]);
    }
    CHECK_SETFIELD(key);
}

/*void SkLua::pushRect(const SkRect& r, const char key[]) {
    lua_newtable(fL);
    setfield_scalar(fL, "left", r.fLeft);
    setfield_scalar(fL, "top", r.fTop);
    setfield_scalar(fL, "right", r.fRight);
    setfield_scalar(fL, "bottom", r.fBottom);
    CHECK_SETFIELD(key);
}*/

/*void SkLua::pushRRect(const SkRRect& rr, const char key[]) {
    push_obj(fL, rr);
    CHECK_SETFIELD(key);
}*/

/*void SkLua::pushDash(const SkPathEffect::DashInfo& info, const char key[]) {
    lua_newtable(fL);
    setfield_scalar(fL, "phase", info.fPhase);
    this->pushArrayScalar(info.fIntervals, info.fCount, "intervals");
    CHECK_SETFIELD(key);
}*/


/*void SkLua::pushMatrix(const SkMatrix& matrix, const char key[]) {
    push_obj(fL, matrix);
    CHECK_SETFIELD(key);
}

void SkLua::pushPaint(const SkPaint& paint, const char key[]) {
    push_obj(fL, paint);
    CHECK_SETFIELD(key);
}

void SkLua::pushPath(const SkPath& path, const char key[]) {
    push_obj(fL, path);
    CHECK_SETFIELD(key);
}

void SkLua::pushCanvas(SkCanvas* canvas, const char key[]) {
    push_ptr(fL, canvas);
    CHECK_SETFIELD(key);
}

void SkLua::pushTextBlob(const SkTextBlob* blob, const char key[]) {
    push_ref(fL, const_cast<SkTextBlob*>(blob));
    CHECK_SETFIELD(key);
}*/

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static SkScalar getfield_scalar(lua_State* L, int index, const char key[]) {
    SkASSERT(lua_istable(L, index));
    lua_pushstring(L, key);
    lua_gettable(L, index);

    SkScalar value = lua2scalar(L, -1);
    lua_pop(L, 1);
    return value;
}

static SkScalar getfield_scalar_default(lua_State* L, int index, const char key[], SkScalar def) {
    SkASSERT(lua_istable(L, index));
    lua_pushstring(L, key);
    lua_gettable(L, index);

    SkScalar value;
    if (lua_isnil(L, -1)) {
        value = def;
    } else {
        value = lua2scalar(L, -1);
    }
    lua_pop(L, 1);
    return value;
}

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

/*static int lpaint_getEffects(lua_State* L) {
    const SkPaint* paint = get_obj<SkPaint>(L, 1);

    lua_newtable(L);
    setfield_bool_if(L, "pathEffect",  !!paint->getPathEffect());
    setfield_bool_if(L, "maskFilter",  !!paint->getMaskFilter());
    setfield_bool_if(L, "shader",      !!paint->getShader());
    setfield_bool_if(L, "colorFilter", !!paint->getColorFilter());
    setfield_bool_if(L, "imageFilter", !!paint->getImageFilter());
    return 1;
}*/


///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////

class AutoCallLua {
public:
    AutoCallLua(lua_State* L, const char func[], const char verb[]) : fL(L) {
        lua_getglobal(L, func);
        if (!lua_isfunction(L, -1)) {
            int t = lua_type(L, -1);
            SkDebugf("--- expected function %d\n", t);
        }

        lua_newtable(L);
        setfield_string(L, "verb", verb);
    }

    ~AutoCallLua() {
        if (lua_pcall(fL, 1, 0, 0) != LUA_OK) {
            SkDebugf("lua err: %s\n", lua_tostring(fL, -1));
        }
        lua_settop(fL, -1);
    }

private:
    lua_State* fL;
};

#define AUTO_LUA(verb)  AutoCallLua acl(fL, fFunc.c_str(), verb)

///////////////////////////////////////////////////////////////////////////////
static bgfx::Init bgfx__init;
static int bgfx_init(lua_State* L){
    push_obj<Init>(L, bgfx__init);
    return 1;
}

static void register_bgfx(lua_State* L) {
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setglobal(L, "bgfx");
    // the Sk table is still on top

    setfield_function(L, "getInit", bgfx_init);
    lua_pop(L, 1);  // pop off the Sk table
}

//======= init =============
static int init_rendererType(lua_State* L){
    auto pInit = get_obj<Init>(L, 1);

    const char* type = lua_tostring(L, -1);
    if(type == NULL){
        //get  //TODO opt init_rendererType
        lua_pushstring(L, getEnumName(pInit->type));
        return 1;
    }
    //set
    bgfx::RendererType::Enum id;
/*#define RENDERER_TYPE_ID(x) else if (strcmp(type, getEnumName(x)) == 0) id = x
    if(0);
    RENDERER_TYPE_ID(bgfx::RendererType::Enum::Count);
    RENDERER_TYPE_ID(bgfx::RendererType::Enum::Noop);
    RENDERER_TYPE_ID(bgfx::RendererType::Enum::Count);
    RENDERER_TYPE_ID(bgfx::RendererType::Enum::Direct3D9);
    RENDERER_TYPE_ID(bgfx::RendererType::Enum::Direct3D11);
    RENDERER_TYPE_ID(bgfx::RendererType::Enum::Direct3D12);
    RENDERER_TYPE_ID(bgfx::RendererType::Enum::Gnm);
    RENDERER_TYPE_ID(bgfx::RendererType::Enum::Metal);
    RENDERER_TYPE_ID(bgfx::RendererType::Enum::Nvn);
    RENDERER_TYPE_ID(bgfx::RendererType::Enum::OpenGLES);
    RENDERER_TYPE_ID(bgfx::RendererType::Enum::OpenGL);
    RENDERER_TYPE_ID(bgfx::RendererType::Enum::Vulkan);
    RENDERER_TYPE_ID(bgfx::RendererType::Enum::WebGPU);
    else
        return luaL_error(L, "Invalid renderer type %s", type);
    pInit->type = id;
    return 0;*/
#define RENDERER_TYPE_ID2(x)  \
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
else return luaL_error(L, "Invalid renderer type %s", type);
    RENDERER_TYPE_ID2(type);
    pInit->type = id;
    return 0;
}

//see 'BGFX_PCI_ID_NONE' and etc
static int init_vendorId(lua_State* L){
    auto pInit = get_obj<Init>(L, 1);
    int type = lua_type(L, -1);
    if(type == LUA_TNIL){
        //nil means get
        lua_pushnumber(L, pInit->vendorId);
        return 1;
    }
    //set
    pInit->vendorId = lua_tointeger(L, -1);
    return 0;
}
static int init_deviceId(lua_State* L){
    auto pInit = get_obj<Init>(L, 1);
    int type = lua_type(L, -1);
    if(type == LUA_TNIL){
        //nil means get
        lua_pushnumber(L, pInit->deviceId);
        return 1;
    }
    pInit->deviceId = lua_tointeger(L, -1);
    return 0;
}
static int init_debug(lua_State* L){
    auto pInit = get_obj<Init>(L, 1);
    int type = lua_type(L, -1);
    if(type == LUA_TNIL){
        //nil means get
        lua_pushboolean(L, pInit->debug ? 1 : 0);
        return 1;
    }
    pInit->debug = lua_toboolean(L, -1) == 1;
    return 0;
}
static int init_profile(lua_State* L){
    auto pInit = get_obj<Init>(L, 1);
    int type = lua_type(L, -1);
    if(type == LUA_TNIL){
        //nil means get
        lua_pushboolean(L, pInit->profile ? 1 : 0);
        return 1;
    }
    pInit->profile = lua_toboolean(L, -1) == 1;
    return 0;
}

//only get
static int init_platformData(lua_State* L){
    auto pInit = get_obj<Init>(L, 1);
    push_obj(L, pInit->platformData);
    return 1;
}

//only get
static int init_resolution(lua_State* L){
    auto pInit = get_obj<Init>(L, 1);
    push_obj(L, pInit->resolution);
    return 1;
}
//only get
static int init_limits(lua_State* L){
    auto pInit = get_obj<Init>(L, 1);
    push_obj(L, pInit->limits);
    return 1;
}

const struct luaL_Reg gInit_Methods[] = {
        //TODO
        { "renderType", init_rendererType},
        { "vendorId", init_vendorId},
        { "deviceId", init_deviceId},
        { "debug", init_debug},
        { "profile", init_profile},
        { "platformData", init_platformData},
        { "resolution", init_resolution},
        { "limits", init_limits},
        {NULL, NULL},
};
//------------ platform-data -----------------------------
#define INIT_PLATFORM_DATA(x) \
static int platformData_##x(lua_State* L){ \
    auto pd = get_obj<PlatformData>(L, 1); \
    auto ud = lua_touserdata(L, -1); \
    if(ud == nullptr){ \
        return luaL_error(L, "Invalid platformData.%s", #x); \
    } \
    pd->x = ud; \
    return 0; \
}
INIT_PLATFORM_DATA(nwh)
INIT_PLATFORM_DATA(ndt)
INIT_PLATFORM_DATA(context)
INIT_PLATFORM_DATA(backBuffer)
INIT_PLATFORM_DATA(backBufferDS)

const struct luaL_Reg gPlatformData_Methods[] = {
        { "nwh", platformData_nwh},
        { "ndt", platformData_ndt},
        { "context", platformData_context},
        { "backBuffer", platformData_backBuffer},
        { "backBufferDS", platformData_backBufferDS},
        {NULL, NULL},
};

//----------------------- resolution ------------------

static const char* resolution_format_str(TextureFormat::Enum en){
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

static int resolution_format(lua_State* L){
    auto reso = get_obj<Resolution>(L, 1);

    const char* type = lua_tostring(L, -1);
    if(type == NULL){
        //get
        lua_pushstring(L, resolution_format_str(reso->format));
        return 1;
    }
    TextureFormat::Enum id; //TODO etc.
#define TYPE_ID(x) \
if (strcmp(x, "Count") == 0) id = TextureFormat::Enum::Count; \
else if (strcmp(x, "BC1") == 0) id = TextureFormat::Enum::BC1; \
else  \
    return luaL_error(L, "known texture format");
    reso->format = id;
    return 0;
}

#define INIT_RESOLUTION(x) \
static int resolution_##x(lua_State* L){ \
    auto pd = get_obj<Resolution>(L, 1); \
    int type = lua_type(L, -1); \
    if(type == LUA_TNIL){ \
        lua_pushnumber(L, pd->x); \
        return 1; \
    } \
    pd->x = lua_tointeger(L, -1); \
    return 0; \
}
INIT_RESOLUTION(width)
INIT_RESOLUTION(height)
INIT_RESOLUTION(reset)
INIT_RESOLUTION(numBackBuffers)
INIT_RESOLUTION(maxFrameLatency)

const struct luaL_Reg gResolution_Methods[] = {
        {"format", resolution_format},
        {"width", resolution_width},
        {"height", resolution_height},
        {"reset", resolution_reset},
        {"numBackBuffers", resolution_numBackBuffers},
        {"maxFrameLatency", resolution_maxFrameLatency},
        {NULL, NULL},
};
//----------------- Limits -----------------
#define INIT_LIMITS(x) \
static int limits_##x(lua_State* L){ \
    auto pd = get_obj<Init::Limits>(L, 1); \
    int type = lua_type(L, -1); \
    if(type == LUA_TNIL){ \
        lua_pushnumber(L, pd->x); \
        return 1; \
    } \
    pd->x = lua_tointeger(L, -1); \
    return 0; \
}
INIT_LIMITS(maxEncoders)
INIT_LIMITS(minResourceCbSize)
INIT_LIMITS(transientVbSize)
INIT_LIMITS(transientIbSize)

namespace gInit{
const struct luaL_Reg Limits_Methods[] = {
        {"maxEncoders", limits_maxEncoders},
        {"minResourceCbSize", limits_minResourceCbSize},
        {"transientVbSize", limits_transientVbSize},
        {"transientIbSize", limits_transientIbSize},
        {NULL, NULL},
};
}
///////////////////////////////////////////////////////////////////////

#define REG_CLASS(L, C)                             \
    do {                                            \
        luaL_newmetatable(L, get_mtname<C>());      \
        lua_pushvalue(L, -1);                       \
        lua_setfield(L, -2, "__index");             \
        luaL_setfuncs(L, g##C##_Methods, 0);        \
        lua_pop(L, 1); /* pop off the meta-table */ \
    } while (0)

void SkLua::Load(lua_State* L) {
    register_bgfx(L);
    REG_CLASS(L, Init);
    REG_CLASS(L, PlatformData);
    REG_CLASS(L, Resolution);
    REG_CLASS(L, Init::Limits);
    //TODO CallbackI*, bx::AllocatorI*
}

extern "C" int luaopen_bgfx(lua_State* L);
extern "C" int luaopen_bgfx(lua_State* L) {
    SkLua::Load(L);
    return 0;
}
