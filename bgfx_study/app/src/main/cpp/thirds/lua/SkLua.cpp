/*
 * Copyright 2013 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <new>
#include <bx/hash.h>
#include <bx/timer.h>
#include <bx/math.h>

#include "SkRefCnt.h"
#include "lua.hpp"

#include "bgfx_wrapper.h"
#include "bgfx_lua_app.h"
#include "SkMemory.h"
#include "bgfx_utils.h"

#include "SkLua.h"
#include "LuaUtils.h"
#include <stdint.h>

#include "int64.h"
#include "SkMemory_lua.h"
#include "bx_lua.h"

extern "C" {
#include <cstring>
}

#define CHECK_MEMORY_VALID(L, m) \
if(m->_ref == 0){ \
    return luaL_error(L, "Invalid sk-memory, which has already been destroyed."); \
}
#define PUT_HANDLE(type) \
auto pHandle = new bgfx::type(); \
pHandle->idx = handle.idx; \
LuaUtils::push_ptr(L, pHandle);

///////////////////////////////////////////////////////////////////////////////

SkLua::SkLua(const char termCode[]) : fTermCode(termCode), fWeOwnL(true) {
    fL = luaL_newstate();
    luaL_openlibs(fL);
    SkLua::Load(fL);
}

SkLua::SkLua(lua_State *L) : fL(L), fWeOwnL(false) {}

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

bool SkLua::runCode(const void *code, size_t size) {
    bx::StringView str((const char *) code, size);
    //SkString str((const char*)code, size);
    return this->runCode(str.getPtr());
}

///////////////////////////////////////////////////////////////////////////////

//---------------------------------------------
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

void SkLua::pushString(const bx::StringView &str, const char key[]) {
    lua_pushstring(fL, str.getPtr());
    CHECK_SETFIELD(key);
}

void SkLua::pushU32(uint32_t value, const char key[]) {
    lua_pushnumber(fL, (double) value);
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
        LuaUtils::setarray_number(fL, i + 1, (double) array[i]);
    }
    CHECK_SETFIELD(key);
}

void SkLua::pushArrayScalar(const SkScalar array[], int count, const char key[]) {
    lua_newtable(fL);
    for (int i = 0; i < count; ++i) {
        // make it base-1 to match lua convention
        LuaUtils::setarray_scalar(fL, i + 1, array[i]);
    }
    CHECK_SETFIELD(key);
}

///////////////////////////////////////////////////////////////////////////////
static int bgfx_getInit(lua_State *L) {
    LOGD("bgfx_getInit:  ");
    LuaUtils::push_ptr<bgfx::Init>(L, getBgfxInit(L));
    return 1;
}

static int bgfx_setDebug(lua_State *L) {
    //luaB_dumpStack(L);
    bgfx::setDebug(TO_NUMBER_32(L, -1));
    return 0;
}

//new app. this should only be called only once.
static int bgfx_newApp(lua_State *L) {
    const char *fn_pre_init = lua_tostring(L, -4);
    const char *fn_init = lua_tostring(L, -3);
    const char *fn_draw = luaL_checkstring(L, -2);
    const char *fn_destroy = lua_tostring(L, -1);
    LuaUtils::push_new<LuaApp>(L, L, fn_pre_init, fn_init, fn_draw, fn_destroy);
    return 1;
}

static int bgfx_setViewClear(lua_State *L) {
    uint8_t _stencil = TO_NUMBER_8(L, -1);
    SkScalar _depth = LuaUtils::lua2scalar(L, -2);
    uint32_t _rgba = TO_NUMBER_32(L, -3);
    uint16_t _flags = TO_NUMBER_16(L, -4);
    uint16_t _id = TO_NUMBER_16(L, -5);
    bgfx::setViewClear(_id, _flags, _rgba, _depth, _stencil);
    return 0;
}

static int bgfx_setViewRect(lua_State *L) {
    auto h = TO_NUMBER_16(L, -1);
    auto w = TO_NUMBER_16(L, -2);
    auto y = TO_NUMBER_16(L, -3);
    auto x = TO_NUMBER_16(L, -4);
    auto _id = TO_NUMBER_16(L, -5);
    bgfx::setViewRect(_id, x, y, w, h);
    return 0;
}

static int bgfx_touch(lua_State *L) {
    uint16_t _id = TO_NUMBER_16(L, -1);
    bgfx::touch(_id);
    return 0;
}

static int bgfx_dbgTextClear(lua_State *L) {
    bgfx::dbgTextClear();
    return 0;
}

static int bgfx_dbgTextImage(lua_State *L) {
    auto x = TO_NUMBER_16(L, -6);
    auto y = TO_NUMBER_16(L, -5);
    auto w = TO_NUMBER_16(L, -4);
    auto h = TO_NUMBER_16(L, -3);
    const char *image = luaL_checkstring(L, -2);
    auto pitch = TO_NUMBER_16(L, -1);
    bgfx::dbgTextImage(x, y, w, h, image, pitch);
    return 0;
}

static int bgfx_dbgTextPrintf(lua_State *L) {
    auto x = TO_NUMBER_16(L, -4);
    auto y = TO_NUMBER_16(L, -3);
    auto attrib = TO_NUMBER_8(L, -2);
    const char *text = luaL_checkstring(L, -1);
    bgfx::dbgTextPrintf(x, y, attrib, "%s", text);
    return 0;
}

static int bgfx_frame(lua_State *L) {
    if (lua_type(L, -1) == LUA_TBOOLEAN) {
        bgfx::frame(lua_toboolean(L, -1) == 1);
    } else {
        bgfx::frame();
    }
    return 0;
}

static int bgfx_getStats(lua_State *L) {
    bgfx::Stats *pStats = const_cast<bgfx::Stats *>(bgfx::getStats());
    LuaUtils::push_ptr(L, pStats);
    return 1;
}

static int bgfx_getCaps(lua_State *L) {
    bgfx::Caps *caps = const_cast<bgfx::Caps *>(bgfx::getCaps());
    LuaUtils::push_ptr(L, caps);
    return 1;
}

static int bgfx_newVertexLayout(lua_State *L) {
    bgfx::VertexLayout *layout = new bgfx::VertexLayout();
    LuaUtils::push_ptr<bgfx::VertexLayout>(L, layout);
    return 1;
}

static void release_Memory(void *_ptr, void *_userData) {
    BX_UNUSED(_ptr);
    IMemory *pMemory = static_cast<IMemory *>(_userData);
    if (pMemory->unRef() == 0) {
        pMemory->destroyData();
    }
}

static int bgfx_makeRef(lua_State *L) {
    SimpleMemory *memory = LuaUtils::to_ref<SkMemory>(L, 1);
    if (memory == nullptr) {
        memory = LuaUtils::to_ref<SkMemoryFFFUI>(L, 1);
    }
    if (memory == nullptr) {
        return luaL_error(L, "bgfx_makeRef failed. %s", lua_tostring(L, 1));
    }
    CHECK_MEMORY_VALID(L, memory)
    memory->ref();
    bgfx::Memory *mem = const_cast<bgfx::Memory *>(bgfx::makeRef(memory->data, memory->size,
                                                                 release_Memory, memory));
    LuaUtils::push_ptr<bgfx::Memory>(L, mem);
    return 1;
}

static int bgfx_createVertexBuffer(lua_State *L) {
    LOGD("bgfx_createVertexBuffer --------------- ");
    luaB_dumpStack(L);
    auto pT = LuaUtils::get_ref<bgfx::Memory>(L, 1);
    auto layout = LuaUtils::get_ref<bgfx::VertexLayout>(L, 2);
    auto flags = lua_tointeger(L, 3);
    bgfx::VertexBufferHandle handle = bgfx::createVertexBuffer(pT, *layout, flags);
    //
    PUT_HANDLE(VertexBufferHandle)
    return 1;
}

static int bgfx_createIndexBuffer(lua_State *L) {
    auto pT = LuaUtils::get_ref<bgfx::Memory>(L, 1);
    auto flags = lua_tointeger(L, 2);
    bgfx::IndexBufferHandle handle = bgfx::createIndexBuffer(pT, flags);

    PUT_HANDLE(IndexBufferHandle)
    return 1;
}

static int bgfx_createProgram(lua_State *L) {
    auto v_sh = LuaUtils::get_ref<bgfx::ShaderHandle>(L, 1);
    auto f_sh = LuaUtils::get_ref<bgfx::ShaderHandle>(L, 2);
    auto destroyShader = lua_type(L, 3) != LUA_TNIL ? lua_toboolean(L, 3) == 1 : false;
    bgfx::ProgramHandle handle = bgfx::createProgram(*v_sh, *f_sh, destroyShader);

    PUT_HANDLE(ProgramHandle)
    return 1;
}

static int bgfx_loadProgram(lua_State *L) {
    auto v_sh = lua_tostring(L, 1);
    auto f_sh = lua_tostring(L, 2);
    bgfx::ProgramHandle handle = loadProgram(v_sh, f_sh);

    PUT_HANDLE(ProgramHandle)
    return 1;
}

static int bgfx_setViewTransform(lua_State *L) {
    // bgfx::setViewTransform(0, view, proj);
    auto id = TO_NUMBER_16(L, 1);
    auto *m1 = LuaUtils::get_ref<SkMemory>(L, 2);
    auto *m2 = LuaUtils::get_ref<SkMemory>(L, 3);
    //m1 ,m2 often should be float* as float-array
    bgfx::setViewTransform(id, m1->data, static_cast<float *>(m2->data));
    return 0;
}

static int bgfx_setTransform(lua_State *L) {
    //bgfx::setTransform
    if (lua_type(L, 1) == LUA_TNUMBER) {
        uint32_t _cache = TO_NUMBER_32(L, 1);
        uint16_t _num = TO_NUMBER_16(L, 2);
        bgfx::setTransform(_cache, _num);
    } else {
        SkMemory *pMemory = LuaUtils::get_ref<SkMemory>(L, 1);
        uint16_t _num = TO_NUMBER_16(L, 2);
        bgfx::setTransform(pMemory->data, _num);
    }
    return 0;
}

static int bgfx_setVertexBuffer(lua_State *L) {
    //bgfx::setVertexBuffer(0, m_vbh)
    uint16_t stream = TO_NUMBER_16(L, 1);
    auto ref = LuaUtils::get_ref<bgfx::VertexBufferHandle>(L, 2);
    //bgfx::VertexBufferHandle handle = { ref->idx };
    bgfx::setVertexBuffer(stream, *ref);
    return 0;
}

static int bgfx_setIndexBuffer(lua_State *L) {
    //bgfx::setIndexBuffer(m_vbh)
    auto ref = LuaUtils::get_ref<bgfx::IndexBufferHandle>(L, 1);
    //bgfx::IndexBufferHandle handle = { ref->idx };
    bgfx::setIndexBuffer(*ref);
    return 0;
}

static int bgfx_setState(lua_State *L) {
    //bgfx::setState(m_vbh)
    uint64_t state = BigUint64(_get(L, 1));
    uint32_t _rgb; //default is zero
    if (lua_type(L, 2) == LUA_TNIL) {
        _rgb = 0;
    } else {
        _rgb = TO_NUMBER_32(L, 2);
    }
    bgfx::setState(state, _rgb);
    return 0;
}

static int bgfx_submit(lua_State *L) {
    //bgfx::submit(
    //		  ViewId _id
    //		, ProgramHandle _program
    //		, uint32_t _depth = 0
    //		, uint8_t _flags  = BGFX_DISCARD_ALL
    //		)
    uint8_t id = TO_NUMBER_8(L, 1);
    bgfx::ProgramHandle *pHandle = LuaUtils::get_ref<bgfx::ProgramHandle>(L, 2);
    uint32_t _depth = lua_type(L, 3) != LUA_TNIL ? TO_NUMBER_32(L, 3) : 0;
    uint8_t _flags =
            lua_type(L, 4) != LUA_TNIL ? TO_NUMBER_8(L, 4) : static_cast<uint8_t>(BGFX_DISCARD_ALL);
    bgfx::submit(id, *pHandle, _depth, _flags);
    return 0;
}

static int bgfx_destroy(lua_State *L) {
    bgfx::IndexBufferHandle *h1 = LuaUtils::to_ref<bgfx::IndexBufferHandle>(L, 1);
    if (h1 != nullptr) {
        bgfx::destroy(*h1);
        return 0;
    }
    bgfx::VertexBufferHandle *h2 = LuaUtils::to_ref<bgfx::VertexBufferHandle>(L, 1);
    if (h2 != nullptr) {
        bgfx::destroy(*h2);
        return 0;
    }
    bgfx::ProgramHandle *h3 = LuaUtils::to_ref<bgfx::ProgramHandle>(L, 1);
    if (h3 != nullptr) {
        bgfx::destroy(*h3);
        return 0;
    }
    return luaL_error(L, "unsupport bgfx::destroy type. %s", lua_tostring(L, 1));
}

static void register_bgfx(lua_State *L) {
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setglobal(L, "bgfx_lua");
    // the bgfx table is still on top

    LuaUtils::setfield_function(L, "getInit", bgfx_getInit);
    lua_pop(L, 1);  // pop off the Sk table
}

//======= init =============
static int init_type(lua_State *L) {
    auto pInit = LuaUtils::get_ref<bgfx::Init>(L, -2);

    LOGD("init_type: pInit = %p", pInit);
    const char *type = lua_tostring(L, -1);
    if (type == NULL) {
        //get
        lua_pushstring(L, bgfx_render_name(L, pInit->type));
        return 1;
    }
    //set
    pInit->type = bgfx_render_enum(L, type);
    return 0;
}

//see 'BGFX_PCI_ID_NONE' and etc
static int init_vendorId(lua_State *L) {
    auto pInit = LuaUtils::get_ref<bgfx::Init>(L, -2);
    int type = lua_type(L, -1);
    if (type == LUA_TNIL) {
        //nil means get
        lua_pushnumber(L, pInit->vendorId);
        return 1;
    }
    //set
    pInit->vendorId = TO_NUMBER_16(L, -1);
    return 0;
}

static int init_deviceId(lua_State *L) {
    auto pInit = LuaUtils::get_ref<bgfx::Init>(L, -2);
    int type = lua_type(L, -1);
    if (type == LUA_TNIL) {
        //nil means get
        lua_pushnumber(L, pInit->deviceId);
        return 1;
    }
    pInit->deviceId = TO_NUMBER_16(L, -1);
    return 0;
}

static int init_debug(lua_State *L) {
    auto pInit = LuaUtils::get_ref<bgfx::Init>(L, -2);
    int type = lua_type(L, -1);
    if (type == LUA_TNIL) {
        //nil means get
        lua_pushboolean(L, pInit->debug ? 1 : 0);
        return 1;
    }
    pInit->debug = lua_toboolean(L, -1) == 1;
    return 0;
}

static int init_profile(lua_State *L) {
    auto pInit = LuaUtils::get_ref<bgfx::Init>(L, -2);
    int type = lua_type(L, -1);
    if (type == LUA_TNIL) {
        //nil means get
        lua_pushboolean(L, pInit->profile ? 1 : 0);
        return 1;
    }
    pInit->profile = lua_toboolean(L, -1) == 1;
    return 0;
}

//only get
static int init_platformData(lua_State *L) {
    auto pInit = LuaUtils::get_ref<bgfx::Init>(L, 1);
    LOGD("init_platformData");
    LuaUtils::push_ptr(L, &pInit->platformData);
    return 1;
}

//only get
static int init_resolution(lua_State *L) {
    auto pInit = LuaUtils::get_ref<bgfx::Init>(L, 1);
    LOGD("init_resolution, init = %p", pInit);
    LuaUtils::push_ptr(L, &pInit->resolution);
    return 1;
}

//only get
static int init_limits(lua_State *L) {
    auto pInit = LuaUtils::get_ref<bgfx::Init>(L, 1);
    LOGD("init_limits");
    LuaUtils::push_ptr(L, &pInit->limits);
    return 1;
}

namespace gbgfx {
    const struct luaL_Reg Init_Methods[] = {
            {"call",         LuaUtils::forward_call},
            {"type",         init_type},
            {"vendorId",     init_vendorId},
            {"deviceId",     init_deviceId},
            {"debug",        init_debug},
            {"profile",      init_profile},
            {"platformData", init_platformData},
            {"resolution",   init_resolution},
            {"limits",       init_limits},
            {NULL, NULL},
    };
}

//------------ platform-data -----------------------------
#define INIT_PLATFORM_DATA(x) \
static int platformData_##x(lua_State* L){ \
    auto pd = LuaUtils::get_ref<bgfx::PlatformData>(L, -2); \
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

namespace gbgfx {
    const struct luaL_Reg PlatformData_Methods[] = {
            {"call",         LuaUtils::forward_call},
            {"nwh",          platformData_nwh},
            {"ndt",          platformData_ndt},
            {"context",      platformData_context},
            {"backBuffer",   platformData_backBuffer},
            {"backBufferDS", platformData_backBufferDS},
            {NULL, NULL},
    };
}

//----------------------- resolution ------------------

static int resolution_format(lua_State *L) {
    auto reso = LuaUtils::get_ref<bgfx::Resolution>(L, -2);

    const char *type = lua_tostring(L, -1);
    if (type == NULL) {
        //get
        lua_pushstring(L, bgfx_textureFormat_name(L, reso->format));
        return 1;
    }
    reso->format = bgfx_textureFormat_enum(L, type);
    return 0;
}

#define INIT_RESOLUTION(x) \
static int resolution_##x(lua_State* L){ \
    auto pd = LuaUtils::get_ref<bgfx::Resolution>(L, -2); \
    int type = lua_type(L, -1); \
    if(type == LUA_TNIL){ \
        lua_pushnumber(L, pd->x); \
        return 1; \
    } \
    pd->x = lua_tointeger(L, -1); \
    LOGD("set resolution_%s, val = %d", #x, pd->x); \
    return 0; \
}

INIT_RESOLUTION(width)

INIT_RESOLUTION(height)

INIT_RESOLUTION(reset)

INIT_RESOLUTION(numBackBuffers)

INIT_RESOLUTION(maxFrameLatency)

namespace gbgfx {
    const struct luaL_Reg Resolution_Methods[] = {
            {"call",            LuaUtils::forward_call},
            {"format",          resolution_format},
            {"width",           resolution_width},
            {"height",          resolution_height},
            {"reset",           resolution_reset},
            {"numBackBuffers",  resolution_numBackBuffers},
            {"maxFrameLatency", resolution_maxFrameLatency},
            {NULL, NULL},
    };
}
//----------------- Limits -----------------
#define INIT_LIMITS(x) \
static int limits_##x(lua_State* L){ \
    auto pd = LuaUtils::get_ref<bgfx::Init::Limits>(L, -2); \
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

namespace gbgfx {
    namespace Init {
        const struct luaL_Reg Limits_Methods[] = {
                {"call",              LuaUtils::forward_call},
                {"maxEncoders",       limits_maxEncoders},
                {"minResourceCbSize", limits_minResourceCbSize},
                {"transientVbSize",   limits_transientVbSize},
                {"transientIbSize",   limits_transientIbSize},
                {NULL, NULL},
        };
    }
}

// =============================== LuaApp =================
static int gc_luaApp(lua_State *L) {
    //here we don't gc lua_app. wait. it really need recycle. draw failed or user-quit.or switch next app
    LOGD("gc_luaApp");
    LuaApp *pApp = LuaUtils::get_obj<LuaApp>(L, 1);
    //pApp->quit();
    return 0;
}

static int destroy_luaApp(lua_State *L) {
    LOGD("destroy_luaApp");
    LuaApp *pApp = LuaUtils::get_obj<LuaApp>(L, 1);
    pApp->quit();
    return 0;
}

static int start_luaApp(lua_State *L) {
    LuaApp *pApp = LuaUtils::get_obj<LuaApp>(L, 1);
    pApp->start();
    return 0;
}

const struct luaL_Reg gLuaApp_Methods[] = {
        {"start",   start_luaApp},
        {"destroy", destroy_luaApp},
        {"__gc",    gc_luaApp},
        {NULL, NULL},
};

//======================= Stats ========================

#define STATS_NUMBER(x) \
static int stats_##x(lua_State * L){ \
    auto pStats = LuaUtils::get_ref<bgfx::Stats>(L, 1); \
    lua_pushnumber(L, pStats->x);\
    return 1; \
}

STATS_NUMBER(cpuTimeFrame);

STATS_NUMBER(cpuTimeBegin);

STATS_NUMBER(cpuTimeEnd);

STATS_NUMBER(cpuTimerFreq);

STATS_NUMBER(gpuTimeBegin);

STATS_NUMBER(gpuTimeEnd);

STATS_NUMBER(gpuTimerFreq);

STATS_NUMBER(waitRender);

STATS_NUMBER(waitSubmit);

STATS_NUMBER(numDraw);

STATS_NUMBER(numCompute);

STATS_NUMBER(numBlit);

STATS_NUMBER(maxGpuLatency);

STATS_NUMBER(numDynamicIndexBuffers);

STATS_NUMBER(numDynamicVertexBuffers);

STATS_NUMBER(numFrameBuffers);

STATS_NUMBER(numIndexBuffers);

STATS_NUMBER(numOcclusionQueries);

STATS_NUMBER(numPrograms);

STATS_NUMBER(numShaders);

STATS_NUMBER(numTextures);

STATS_NUMBER(numUniforms);

STATS_NUMBER(numVertexBuffers);

STATS_NUMBER(numVertexLayouts);

STATS_NUMBER(textureMemoryUsed);

STATS_NUMBER(rtMemoryUsed);

STATS_NUMBER(transientVbUsed);

STATS_NUMBER(transientIbUsed);

STATS_NUMBER(gpuMemoryMax);

STATS_NUMBER(gpuMemoryUsed);

STATS_NUMBER(width);

STATS_NUMBER(height);

STATS_NUMBER(textWidth);

STATS_NUMBER(textHeight);

STATS_NUMBER(numViews);

STATS_NUMBER(numEncoders);
//TODO ViewStats, EncoderStats

namespace gbgfx {
    const struct luaL_Reg Stats_Methods[] = {
            {"call",                    LuaUtils::forward_call},
            {"cpuTimeFrame",            stats_cpuTimeFrame},
            {"cpuTimeBegin",            stats_cpuTimeBegin},
            {"cpuTimeEnd",              stats_cpuTimeEnd},
            {"cpuTimerFreq",            stats_cpuTimerFreq},
            {"gpuTimeBegin",            stats_gpuTimeBegin},
            {"gpuTimeEnd",              stats_gpuTimeEnd},
            {"gpuTimerFreq",            stats_gpuTimerFreq},
            {"waitRender",              stats_waitRender},
            {"waitSubmit",              stats_waitSubmit},

            {"numDraw",                 stats_numDraw},
            {"numCompute",              stats_numCompute},
            {"numBlit",                 stats_numBlit},
            {"maxGpuLatency",           stats_maxGpuLatency},

            {"numDynamicIndexBuffers",  stats_numDynamicIndexBuffers},
            {"numDynamicVertexBuffers", stats_numDynamicVertexBuffers},
            {"numFrameBuffers",         stats_numFrameBuffers},
            {"numIndexBuffers",         stats_numIndexBuffers},
            {"numOcclusionQueries",     stats_numOcclusionQueries},
            {"numPrograms",             stats_numPrograms},
            {"numShaders",              stats_numShaders},
            {"numTextures",             stats_numTextures},
            {"numUniforms",             stats_numUniforms},
            {"numVertexBuffers",        stats_numVertexBuffers},
            {"numVertexLayouts",        stats_numVertexLayouts},

            {"textureMemoryUsed",       stats_textureMemoryUsed},
            {"rtMemoryUsed",            stats_rtMemoryUsed},
            {"transientVbUsed",         stats_transientVbUsed},
            {"transientIbUsed",         stats_transientIbUsed},
            {"gpuMemoryMax",            stats_gpuMemoryMax},
            {"gpuMemoryUsed",           stats_gpuMemoryUsed},

            {"width",                   stats_width},
            {"height",                  stats_height},
            {"textWidth",               stats_textWidth},
            {"textHeight",              stats_textHeight},
            {"numViews",                stats_numViews},
            {"numEncoders",             stats_numEncoders},
            {NULL, NULL},
    };
}

//--------------------- bgfx::Caps -----------------------
static int caps_renderType(lua_State *L) {
    bgfx::Caps *pCaps = LuaUtils::get_ref<bgfx::Caps>(L, 1);
    const char *type = bgfx_render_name(L, pCaps->rendererType);
    lua_pushstring(L, type);
    return 1;
}

static int caps_homogeneousDepth(lua_State *L) {
    bgfx::Caps *pCaps = LuaUtils::get_ref<bgfx::Caps>(L, 1);
    lua_pushboolean(L, pCaps->homogeneousDepth ? 1 : 0);
    return 1;
}

static int caps_originBottomLeft(lua_State *L) {
    bgfx::Caps *pCaps = LuaUtils::get_ref<bgfx::Caps>(L, 1);
    lua_pushboolean(L, pCaps->originBottomLeft ? 1 : 0);
    return 1;
}

static int caps_numGPUs(lua_State *L) {
    bgfx::Caps *pCaps = LuaUtils::get_ref<bgfx::Caps>(L, 1);
    lua_pushinteger(L, pCaps->numGPUs);
    return 1;
}

static int caps_formats(lua_State *L) {
    bgfx::Caps *pCaps = LuaUtils::get_ref<bgfx::Caps>(L, 1);
    lua_newtable(L);
    for (int i = 0, size = sizeof(pCaps->formats) / pCaps->formats[0]; i < size; ++i) {
        // make it base-1 to match lua convention
        LuaUtils::setarray_number(L, i + 1, pCaps->formats[i]);
    }
    return 1;
}

namespace gbgfx {
    const struct luaL_Reg Caps_Methods[] = {
            {"call",             LuaUtils::forward_call},
            {"rendererType",     caps_renderType},
            {"homogeneousDepth", caps_homogeneousDepth},
            {"originBottomLeft", caps_originBottomLeft},
            {"numGPUs",          caps_numGPUs},
            {"formats",          caps_formats},
            {NULL, NULL},
    };
}
//---------------------- VertexLayout ----------------------

static int vertexLayout_begin(lua_State *L) {
    LOGD("vertexLayout_begin------");
    luaB_dumpStack(L);

    bgfx::VertexLayout *pLayout = LuaUtils::get_ref<bgfx::VertexLayout>(L, 1);
    //RendererType::Enum
    const char *name = lua_tostring(L, 2);
    if (name == nullptr) {
        pLayout->begin();
    } else {
        pLayout->begin(bgfx_render_enum(L, name));
    }
    //return this
    lua_pushvalue(L, 1);
    return 1;
}

static int vertexLayout_add(lua_State *L) {
    bgfx::VertexLayout *pLayout = LuaUtils::get_ref<bgfx::VertexLayout>(L, 1);
    //.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
    bgfx::Attrib::Enum attrib = bgfx_attrib_enum(L, lua_tostring(L, 2));
    auto _num = TO_NUMBER_8(L, 3);
    bgfx::AttribType::Enum attribType = bgfx_attribType_enum(L, lua_tostring(L, 4));
    //opt parameter
    auto _normalized = lua_type(L, 5) != LUA_TNIL ? lua_toboolean(L, 5) == 1 : false;
    auto _asInt = lua_type(L, 6) != LUA_TNIL ? lua_toboolean(L, 6) == 1 : false;

    pLayout->add(attrib, _num, attribType, _normalized, _asInt);
    //return this
    lua_pushvalue(L, 1);
    return 1;
}

static int vertexLayout_skip(lua_State *L) {
    bgfx::VertexLayout *pLayout = LuaUtils::get_ref<bgfx::VertexLayout>(L, 1);
    auto _num = TO_NUMBER_8(L, 2);
    pLayout->skip(_num);
    //return this
    lua_pushvalue(L, 1);
    return 1;
}

static int vertexLayout_decode(lua_State *L) {
    bgfx::VertexLayout *pLayout = LuaUtils::get_ref<bgfx::VertexLayout>(L, 1);
    //.decode(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
    bgfx::Attrib::Enum attrib = bgfx_attrib_enum(L, lua_tostring(L, 2));
    auto _num = TO_NUMBER_8(L, 3);
    bgfx::AttribType::Enum attribType = bgfx_attribType_enum(L, lua_tostring(L, 4));
    auto _normalized = lua_toboolean(L, 5) == 1;
    auto _asInt = lua_toboolean(L, 6) == 1;
    pLayout->decode(attrib, _num, attribType, _normalized, _asInt);
    //return this
    lua_pushvalue(L, 1);
    return 1;
}

static int vertexLayout_has(lua_State *L) {
    bgfx::VertexLayout *pLayout = LuaUtils::get_ref<bgfx::VertexLayout>(L, 1);
    //bool has(Attrib::Enum _attrib);
    bgfx::Attrib::Enum attrib = bgfx_attrib_enum(L, lua_tostring(L, 2));
    lua_pushboolean(L, pLayout->has(attrib) ? 1 : 0);
    return 1;
}

static int vertexLayout_getOffset(lua_State *L) {
    bgfx::VertexLayout *pLayout = LuaUtils::get_ref<bgfx::VertexLayout>(L, 1);
    bgfx::Attrib::Enum attrib = bgfx_attrib_enum(L, lua_tostring(L, 2));
    //uint16_t getOffset(Attrib::Enum _attrib)
    lua_pushnumber(L, pLayout->getOffset(attrib));
    return 1;
}

static int vertexLayout_getStride(lua_State *L) {
    bgfx::VertexLayout *pLayout = LuaUtils::get_ref<bgfx::VertexLayout>(L, 1);
    //uint16_t getStride() const { return m_stride; }
    lua_pushnumber(L, pLayout->getStride());
    return 1;
}

static int vertexLayout_getSize(lua_State *L) {
    bgfx::VertexLayout *pLayout = LuaUtils::get_ref<bgfx::VertexLayout>(L, 1);
    //uint32_t getSize(uint32_t _num);
    auto _num = TO_NUMBER_8(L, 2);
    lua_pushnumber(L, pLayout->getSize(_num));
    return 1;
}

static int vertexLayout_end(lua_State *L) {
    bgfx::VertexLayout *pLayout = LuaUtils::get_ref<bgfx::VertexLayout>(L, 1);
    pLayout->end();
    return 0;
}

namespace gbgfx {
    const static luaL_Reg VertexLayout_Methods[] = {
            {"call",      LuaUtils::forward_func_call},
            {"begin",     vertexLayout_begin},
            {"add",       vertexLayout_add},
            {"skip",      vertexLayout_skip},
            {"decode",    vertexLayout_decode},
            {"has",       vertexLayout_has},
            {"getOffset", vertexLayout_getOffset},
            {"getStride", vertexLayout_getStride},
            {"getSize",   vertexLayout_getSize},
            {"ends",      vertexLayout_end}, // end is key-world of lua
            {NULL, NULL},
    };
}


inline bgfx::Init *getBgfxInit(lua_State *L) {
    return Bgfx_lua_app::requireInit(L);
}

// here we direct load bgfx_lua.
static const luaL_Reg bgfx_funcs[] = {
        {"getInit",            bgfx_getInit},
        {"newApp",             bgfx_newApp},
        {"setDebug",           bgfx_setDebug},
        {"setViewClear",       bgfx_setViewClear},
        {"setViewRect",        bgfx_setViewRect},
        {"touch",              bgfx_touch},
        {"dbgTextClear",       bgfx_dbgTextClear},
        {"dbgTextImage",       bgfx_dbgTextImage},
        {"dbgTextPrintf",      bgfx_dbgTextPrintf},
        {"frame",              bgfx_frame},
        {"getStats",           bgfx_getStats},
        {"getCaps",            bgfx_getCaps},

        {"newVertexLayout",    bgfx_newVertexLayout},
        {"makeRef",            bgfx_makeRef},
        {"createVertexBuffer", bgfx_createVertexBuffer},
        {"createIndexBuffer",  bgfx_createIndexBuffer},
        {"createProgram",      bgfx_createProgram},
        {"setViewTransform",   bgfx_setViewTransform},
        {"setTransform",       bgfx_setTransform},

        {"setVertexBuffer",    bgfx_setVertexBuffer},
        {"setIndexBuffer",     bgfx_setIndexBuffer},
        {"setState",           bgfx_setState},
        {"submit",             bgfx_submit},
        {"destroy",            bgfx_destroy},

        //----- extra relative bgfx stand api ------
        {"loadProgram",        bgfx_loadProgram},
        {nullptr,              nullptr}
};
extern "C" int luaopen_bgfx_lua(lua_State *L) {
    SkLua::Load(L);
    luaL_newlib(L, bgfx_funcs);
    return 1;
}


///////////////////////////////////////////////////////////////////////

#define HANDLE_METHODS(t) \
static int t##_gc(lua_State* L){ \
    auto pT = LuaUtils::get_ref<bgfx::t>(L, 1); \
    LOGD("handle is recycled."); \
    delete pT;\
    return 0;\
}\
namespace gbgfx{\
    static const luaL_Reg t##_Methods[] = {\
            {"__gc", t##_gc},\
            {nullptr, nullptr}\
    };\
}

HANDLE_METHODS(VertexBufferHandle)
HANDLE_METHODS(VertexLayoutHandle)
HANDLE_METHODS(IndexBufferHandle)
HANDLE_METHODS(ProgramHandle)
HANDLE_METHODS(ShaderHandle)

DEF_MTNAME(bgfx::Init)
DEF_MTNAME(bgfx::PlatformData)
DEF_MTNAME(bgfx::Resolution)
DEF_MTNAME(bgfx::Init::Limits)
DEF_MTNAME(bgfx::Stats)
DEF_MTNAME(bgfx::Caps)
DEF_MTNAME(bgfx::VertexLayout)

DEF_MTNAME(LuaApp)
//----------- only get_mtname ------
DEF_MTNAME(bgfx::Memory)
DEF_MTNAME(bgfx::VertexBufferHandle)
DEF_MTNAME(bgfx::VertexLayoutHandle)
DEF_MTNAME(bgfx::IndexBufferHandle)
DEF_MTNAME(bgfx::ProgramHandle)
DEF_MTNAME(bgfx::ShaderHandle)

void SkLua::Load(lua_State *L) {
    //register_bgfx(L);
    REG_CLASS(L, bgfx::Init);
    REG_CLASS(L, bgfx::PlatformData);
    REG_CLASS(L, bgfx::Resolution);
    REG_CLASS(L, bgfx::Init::Limits);
    REG_CLASS(L, bgfx::Stats);
    REG_CLASS(L, bgfx::Caps);
    REG_CLASS(L, bgfx::VertexLayout);

    REG_CLASS(L, LuaApp);

    REG_EMPTY_CLASS(L, bgfx::Memory);
    REG_CLASS(L, bgfx::VertexBufferHandle);
    REG_CLASS(L, bgfx::VertexLayoutHandle);
    REG_CLASS(L, bgfx::IndexBufferHandle);
    REG_CLASS(L, bgfx::ProgramHandle);
    REG_CLASS(L, bgfx::ShaderHandle);
    //TODO Callback
    SkMemoryLua::registers(L);
    BxLua::registers(L);
}