/*
 * Copyright 2013 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <new>
#include <bx/hash.h>
#include <bx/timer.h>
#include "SkLua.h"
#include "SkRefCnt.h"
#include "lua.hpp"
#include "bgfx_wrapper.h"
#include "bgfx_lua_app.h"
#include "SkMemory.h"
#include "bgfx_utils.h"

extern "C"{
#include <cstring>
}

using namespace bgfx;

#define CHECK_MEMORY_VALID(L, m) \
if(m->_ref == 0){ \
    return luaL_error(L, "Invalid sk-memory, which has already been destroyed."); \
}

// return the metatable name for a given class
template <typename T> const char* get_mtname();
#define DEF_MTNAME(T)                           \
    template <> const char* get_mtname<T>() {   \
        return #T "_LuaMetaTableName";          \
    }

DEF_MTNAME(LuaApp)
DEF_MTNAME(Init)
DEF_MTNAME(PlatformData)
DEF_MTNAME(Resolution)
DEF_MTNAME(Init::Limits)
DEF_MTNAME(Stats)
DEF_MTNAME(VertexLayout)
DEF_MTNAME(SkMemory)
DEF_MTNAME(SkMemoryFFFUI)
//----------- only get_mtname ------
DEF_MTNAME(Memory)
DEF_MTNAME(VertexBufferHandle)
DEF_MTNAME(VertexLayoutHandle)
DEF_MTNAME(IndexBufferHandle)
DEF_MTNAME(ProgramHandle)
DEF_MTNAME(ShaderHandle)

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
    T** ls = (T**)lua_newuserdata(L, sizeof(T*));
    *ls = ptr;
    //LOGD("push_ptr:  ls = %p, ptr = %p", ls, ptr);
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
static int bgfx_getInit(lua_State* L){
    LOGD("bgfx_getInit:  ");
    push_ptr<Init>(L, getBgfxInit(L));
    return 1;
}
static int bgfx_setDebug(lua_State* L){
    //luaB_dumpStack(L);
    bgfx::setDebug(TO_NUMBER_32(L, -1));
    return 0;
}
//new app. this should only be called only once.
static int bgfx_newApp(lua_State* L){
    const char* fn_pre_init = luaL_checkstring(L, -4);
    const char* fn_init = luaL_checkstring(L, -3);
    const char* fn_draw =luaL_checkstring(L, -2);
    const char* fn_destroy =luaL_checkstring(L, -1);
    push_new<LuaApp>(L, L, fn_pre_init,  fn_init, fn_draw, fn_destroy);
    return 1;
}
static int bgfx_setViewClear(lua_State* L){
    uint8_t  _stencil = TO_NUMBER_8(L, -1);
    SkScalar  _depth = lua2scalar(L, -2);
    uint32_t  _rgba = TO_NUMBER_32(L, -3);
    uint16_t  _flags = TO_NUMBER_16(L, -4);
    uint16_t  _id = TO_NUMBER_16(L, -5);
    bgfx::setViewClear(_id, _flags, _rgba, _depth, _stencil);
    return 0;
}

static int bgfx_setViewRect(lua_State* L){
    auto  h = TO_NUMBER_16(L, -1);
    auto  w = TO_NUMBER_16(L, -2);
    auto  y = TO_NUMBER_16(L, -3);
    auto  x = TO_NUMBER_16(L, -4);
    auto  _id = TO_NUMBER_16(L, -5);
    bgfx::setViewRect(_id, x, y, w, h);
    return 0;
}
static int bgfx_touch(lua_State* L){
    uint16_t  _id = TO_NUMBER_16(L, -1);
    bgfx::touch(_id);
    return 0;
}

static int bgfx_dbgTextClear(lua_State* L){
    bgfx::dbgTextClear();
    return 0;
}
static int bgfx_dbgTextImage(lua_State* L){
    auto x = TO_NUMBER_16(L, -6);
    auto y = TO_NUMBER_16(L, -5);
    auto w = TO_NUMBER_16(L, -4);
    auto h = TO_NUMBER_16(L, -3);
    const char * image = luaL_checkstring(L, -2);
    auto pitch = TO_NUMBER_16(L, -1);
    bgfx::dbgTextImage(x, y, w, h, image, pitch);
    return 0;
}

static int bgfx_dbgTextPrintf(lua_State *L) {
    auto x = TO_NUMBER_16(L, -4);
    auto y = TO_NUMBER_16(L, -3);
    auto attrib = TO_NUMBER_8(L, -2);
    const char * text = luaL_checkstring(L, -1);
    bgfx::dbgTextPrintf(x, y, attrib,"%s",text);
    return 0;
}
static int bgfx_frame(lua_State *L) {
    if(lua_type(L, -1) == LUA_TBOOLEAN){
        bgfx::frame(lua_toboolean(L, -1) == 1);
    } else{
        bgfx::frame();
    }
    return 0;
}

static int bgfx_getStats(lua_State *L) {
    Stats* pStats = const_cast<Stats *>(bgfx::getStats());
    push_ptr(L, pStats);
    return 1;
}

static int bgfx_newVertexLayout(lua_State *L){
    VertexLayout *layout = new VertexLayout();
    push_ptr(L, layout);
    return 1;
}

static int bgfx_newMemory(lua_State *L){
    //int tableCount, type (b, s, i, f)
    const char* type = lua_tostring(L, -1);
    int tableCount = static_cast<int>(lua_tointeger(L, -2));
    lua_pop(L, 2);
    SkMemory *pMemory = new SkMemory(L, tableCount, type);
    push_ptr(L, pMemory);
    return 1;
}
static int bgfx_newMemoryFFFUI(lua_State *L){
    //int tableCount, type (b, s, i, f)
    int tableCount = static_cast<int>(lua_tointeger(L, -1));
    lua_pop(L, 1);
    SkMemoryFFFUI *pMemory = new SkMemoryFFFUI(L, tableCount);
    push_ptr(L, pMemory);
    return 1;
}
static void release_Memory(void* _ptr, void* _userData){
    BX_UNUSED(_ptr);
    AbsSkMemory* pMemory = static_cast<AbsSkMemory *>(_userData);
    if(pMemory->unRef() == 0){
        pMemory->destroyData();
    }
}
static int bgfx_makeRef(lua_State* L){
    AbsSkMemory * memory = get_ref<SkMemory>(L, 1);
    if(memory == nullptr){
        memory = get_ref<SkMemoryFFFUI>(L, 1);
    }
    if(memory == nullptr){
        return luaL_error(L, "bgfx_makeRef failed. %s", lua_tostring(L, 1));
    }
    CHECK_MEMORY_VALID(L, memory)
    memory->ref();
    Memory* mem = const_cast<Memory *>(bgfx::makeRef(memory->data, memory->size,
                                                     release_Memory, memory));
    push_ptr(L, mem);
    return 1;
}

static int bgfx_createVertexBuffer(lua_State* L){
    auto pT = get_ref<Memory>(L, 1);
    auto layout = get_ref<VertexLayout>(L, 2);
    auto flags = lua_tointeger(L, 3);
    VertexBufferHandle handle = bgfx::createVertexBuffer(pT, *layout, flags);
    push_ptr(L, &handle);
    return 1;
}
static int bgfx_createIndexBuffer(lua_State* L){
    auto pT = get_ref<Memory>(L, 1);
    auto flags = lua_tointeger(L, 2);
    IndexBufferHandle handle = bgfx::createIndexBuffer(pT, flags);
    push_ptr(L, &handle);
    return 1;
}

static int bgfx_createProgram(lua_State* L){
    auto v_sh = get_ref<ShaderHandle>(L, 1);
    auto f_sh = get_ref<ShaderHandle>(L, 2);
    auto destroyShader = lua_toboolean(L, 3) == 1;
    ProgramHandle handle = bgfx::createProgram(*v_sh, *f_sh, destroyShader);
    push_ptr(L, &handle);
    return 1;
}

static int bgfx_loadProgram(lua_State* L){
    auto v_sh = lua_tostring(L, 1);
    auto f_sh = lua_tostring(L, 2);
    ProgramHandle handle = loadProgram(v_sh, f_sh);
    push_ptr(L, &handle);
    return 1;
}

static void register_bgfx(lua_State* L) {
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setglobal(L, "bgfx_lua");
    // the bgfx table is still on top

    setfield_function(L, "getInit", bgfx_getInit);
    setfield_function(L, "newApp", bgfx_newApp);

    setfield_function(L, "setDebug", bgfx_setDebug);

    setfield_function(L, "setViewClear", bgfx_setViewClear);
    setfield_function(L, "setViewRect", bgfx_setViewRect);
    setfield_function(L, "touch", bgfx_touch);
    setfield_function(L, "dbgTextClear", bgfx_dbgTextClear);
    setfield_function(L, "dbgTextImage", bgfx_dbgTextImage);
    setfield_function(L, "dbgTextPrintf", bgfx_dbgTextPrintf);
    setfield_function(L, "frame", bgfx_frame);
    setfield_function(L, "getStats", bgfx_getStats);
    lua_pop(L, 1);  // pop off the Sk table
}

//======= init =============
static int init_type(lua_State* L) {
    auto pInit = get_ref<Init>(L, -2);

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
static int init_vendorId(lua_State* L){
    auto pInit = get_ref<Init>(L, -2);
    int type = lua_type(L, -1);
    if(type == LUA_TNIL){
        //nil means get
        lua_pushnumber(L, pInit->vendorId);
        return 1;
    }
    //set
    pInit->vendorId = TO_NUMBER_16(L, -1);
    return 0;
}
static int init_deviceId(lua_State* L){
    auto pInit = get_ref<Init>(L, -2);
    int type = lua_type(L, -1);
    if(type == LUA_TNIL){
        //nil means get
        lua_pushnumber(L, pInit->deviceId);
        return 1;
    }
    pInit->deviceId = TO_NUMBER_16(L, -1);
    return 0;
}
static int init_debug(lua_State* L){
    auto pInit = get_ref<Init>(L, -2);
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
    auto pInit = get_ref<Init>(L, -2);
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
    auto pInit = get_ref<Init>(L, 1);
    LOGD("init_platformData");
    push_ptr(L, &pInit->platformData);
    return 1;
}

//only get
static int init_resolution(lua_State* L){
    auto pInit = get_ref<Init>(L, 1);
    LOGD("init_resolution, init = %p", pInit);
    push_ptr(L, &pInit->resolution);
    return 1;
}
//only get
static int init_limits(lua_State* L){
    auto pInit = get_ref<Init>(L, 1);
    LOGD("init_limits");
    push_ptr(L, &pInit->limits);
    return 1;
}
//wrap method for lua get and set. see 'func_to_getset.lua'
static int forward_call(lua_State* L){ //a.call(a, k, v)
    const char *key = lua_tostring(L, -2);
    // before is {a, k, v};
    lua_pushvalue(L, 2); //{a, k, v, k}
    lua_gettable(L, 1); // {a, k, v, method}
    lua_pushvalue(L, 1); // {a, k, v, method, a}
    lua_pushvalue(L, 3); // {a, k, v, method, a, v}
    LOGD("---------  ---------");
    LOGD("before call... %s", key);
    luaB_dumpStack(L);
    int result = lua_pcall(L, 2, 1, 0); //two arguments, 1 result,
    LOGD("after call...");
    luaB_dumpStack(L);
    LOGD("---------  ---------");
    if(result == LUA_OK){
        lua_pushvalue(L, -1);
        return 1;
    }
    return 0;
}

const struct luaL_Reg gInit_Methods[] = {
        { "call", forward_call},
        { "type", init_type},
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
    auto pd = get_ref<PlatformData>(L, -2); \
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
        { "call", forward_call},
        { "nwh", platformData_nwh},
        { "ndt", platformData_ndt},
        { "context", platformData_context},
        { "backBuffer", platformData_backBuffer},
        { "backBufferDS", platformData_backBufferDS},
        {NULL, NULL},
};

//----------------------- resolution ------------------

static int resolution_format(lua_State* L){
    auto reso = get_ref<Resolution>(L, -2);

    const char* type = lua_tostring(L, -1);
    if(type == NULL){
        //get
        lua_pushstring(L, bgfx_textureFormat_name(L, reso->format));
        return 1;
    }
    reso->format = bgfx_textureFormat_enum(L, type);
    return 0;
}

#define INIT_RESOLUTION(x) \
static int resolution_##x(lua_State* L){ \
    auto pd = get_ref<Resolution>(L, -2); \
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

const struct luaL_Reg gResolution_Methods[] = {
        {"call", forward_call},
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
    auto pd = get_ref<Init::Limits>(L, -2); \
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
        {"call", forward_call},
        {"maxEncoders", limits_maxEncoders},
        {"minResourceCbSize", limits_minResourceCbSize},
        {"transientVbSize", limits_transientVbSize},
        {"transientIbSize", limits_transientIbSize},
        {NULL, NULL},
};
}
// =============================== LuaApp =================
static int gc_luaApp(lua_State * L){
    LOGD("gc_luaApp");
    LuaApp *pApp = get_obj<LuaApp>(L, 1);
    pApp->quit();
    return 0;
}
static int destroy_luaApp(lua_State * L){
    LOGD("destroy_luaApp");
    LuaApp *pApp = get_obj<LuaApp>(L, 1);
    pApp->quit();
    return 0;
}
static int start_luaApp(lua_State * L){
    LuaApp *pApp = get_obj<LuaApp>(L, 1);
    pApp->start();
    return 0;
}
const struct luaL_Reg gLuaApp_Methods[] = {
        {"start", start_luaApp},
        {"destroy", destroy_luaApp},
        {"__gc", gc_luaApp},
        {NULL, NULL},
};

//======================= Stats ========================

#define STATS_NUMBER(x) \
static int stats_##x(lua_State * L){ \
    auto pStats = get_ref<Stats>(L, 1); \
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

const struct luaL_Reg gStats_Methods[] = {
        {"call", forward_call},
        {"cpuTimeFrame", stats_cpuTimeFrame},
        {"cpuTimeBegin", stats_cpuTimeBegin},
        {"cpuTimeEnd", stats_cpuTimeEnd},
        {"cpuTimerFreq", stats_cpuTimerFreq},
        {"gpuTimeBegin", stats_gpuTimeBegin},
        {"gpuTimeEnd", stats_gpuTimeEnd},
        {"gpuTimerFreq", stats_gpuTimerFreq},
        {"waitRender", stats_waitRender},
        {"waitSubmit", stats_waitSubmit},

        {"numDraw", stats_numDraw},
        {"numCompute", stats_numCompute},
        {"numBlit", stats_numBlit},
        {"maxGpuLatency", stats_maxGpuLatency},

        {"numDynamicIndexBuffers", stats_numDynamicIndexBuffers},
        {"numDynamicVertexBuffers", stats_numDynamicVertexBuffers},
        {"numFrameBuffers", stats_numFrameBuffers},
        {"numIndexBuffers", stats_numIndexBuffers},
        {"numOcclusionQueries", stats_numOcclusionQueries},
        {"numPrograms", stats_numPrograms},
        {"numShaders", stats_numShaders},
        {"numTextures", stats_numTextures},
        {"numUniforms", stats_numUniforms},
        {"numVertexBuffers", stats_numVertexBuffers},
        {"numVertexLayouts", stats_numVertexLayouts},

        {"textureMemoryUsed", stats_textureMemoryUsed},
        {"rtMemoryUsed", stats_rtMemoryUsed},
        {"transientVbUsed", stats_transientVbUsed},
        {"transientIbUsed", stats_transientIbUsed},
        {"gpuMemoryMax", stats_gpuMemoryMax},
        {"gpuMemoryUsed", stats_gpuMemoryUsed},

        {"width", stats_width},
        {"height", stats_height},
        {"textWidth", stats_textWidth},
        {"textHeight", stats_textHeight},
        {"numViews", stats_numViews},
        {"numEncoders", stats_numEncoders},
        {NULL, NULL},
};
//---------------------- VertexLayout ----------------------

static int vertexLayout_begin(lua_State* L){
    VertexLayout *pLayout = get_ref<VertexLayout>(L, 1);
    //RendererType::Enum
    const char *name = lua_tostring(L, -1);
    if(name == nullptr){
        pLayout->begin();
    } else{
        pLayout->begin(bgfx_render_enum(L, name));
    }
    //return this
    lua_pushvalue(L, 1);
    return 1;
}

static int vertexLayout_add(lua_State* L){
    VertexLayout *pLayout = get_ref<VertexLayout>(L, 1);
    //.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
    auto _num = TO_NUMBER_8(L, 3);
    auto _normalized = lua_toboolean(L, 4) == 1;
    auto _asInt = lua_toboolean(L, 5) == 1;

    Attrib::Enum attrib = bgfx_attrib_enum(L, lua_tostring(L, 2));
    AttribType::Enum attribType = bgfx_attribType_enum(L, lua_tostring(L, 4));
    pLayout->add(attrib, _num, attribType, _normalized, _asInt);
    //return this
    lua_pushvalue(L, 1);
    return 1;
}

static int vertexLayout_skip(lua_State* L){
    VertexLayout *pLayout = get_ref<VertexLayout>(L, 1);
    auto _num = TO_NUMBER_8(L, -1);
    pLayout->skip(_num);
    //return this
    lua_pushvalue(L, 1);
    return 1;
}

static int vertexLayout_decode(lua_State* L){
    VertexLayout *pLayout = get_ref<VertexLayout>(L, 1);
    //.decode(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
    auto _num = TO_NUMBER_8(L, 3);
    auto _normalized = lua_toboolean(L, 4) == 1;
    auto _asInt = lua_toboolean(L, 5) == 1;

    Attrib::Enum attrib = bgfx_attrib_enum(L, lua_tostring(L, 2));
    AttribType::Enum attribType = bgfx_attribType_enum(L, lua_tostring(L, 4));
    pLayout->decode(attrib, _num, attribType, _normalized, _asInt);
    //return this
    lua_pushvalue(L, 1);
    return 1;
}
static int vertexLayout_has(lua_State* L){
    VertexLayout *pLayout = get_ref<VertexLayout>(L, 1);
    //bool has(Attrib::Enum _attrib);
    Attrib::Enum attrib = bgfx_attrib_enum(L, lua_tostring(L, -1));
    lua_pushboolean(L, pLayout->has(attrib) ? 1 : 0);
    return 1;
}
static int vertexLayout_getOffset(lua_State* L){
    VertexLayout *pLayout = get_ref<VertexLayout>(L, 1);
    Attrib::Enum attrib = bgfx_attrib_enum(L, lua_tostring(L, -1));
    //uint16_t getOffset(Attrib::Enum _attrib)
    lua_pushnumber(L, pLayout->getOffset(attrib));
    return 1;
}
static int vertexLayout_getStride(lua_State* L){
    VertexLayout *pLayout = get_ref<VertexLayout>(L, 1);
    //uint16_t getStride() const { return m_stride; }
    lua_pushnumber(L, pLayout->getStride());
    return 1;
}
static int vertexLayout_getSize(lua_State* L){
    VertexLayout *pLayout = get_ref<VertexLayout>(L, 1);
    //uint32_t getSize(uint32_t _num);
    auto _num = TO_NUMBER_8(L, -1);
    lua_pushnumber(L, pLayout->getSize(_num));
    return 1;
}

static int vertexLayout_end(lua_State* L){
    VertexLayout *pLayout = get_ref<VertexLayout>(L, 1);
    pLayout->end();
    return 0;
}

const static luaL_Reg gVertexLayout_Methods[] = {
        {"begin", vertexLayout_begin},
        {"add", vertexLayout_add},
        {"skip", vertexLayout_skip},
        {"decode", vertexLayout_decode},
        {"has", vertexLayout_has},
        {"getOffset", vertexLayout_getOffset},
        {"getStride", vertexLayout_getStride},
        {"getSize", vertexLayout_getSize},
        {"ends", vertexLayout_end}, // end is key-world of lua
        {NULL, NULL},
};
//---------------------- Memory --------------------------
#define memory_isValid(type)  \
static int type##_isValid(lua_State* L){ \
    auto pMemory = get_ref<type>(L, 1); \
    lua_pushboolean(L, pMemory->_ref > 0); \
    return 1; \
}
#define memory_gc(type) \
static int type##_gc(lua_State* L){ \
    auto pMemory = get_ref<type>(L, 1); \
    if(pMemory->unRef() == 0){ \
        pMemory->destroyData(); \
        delete pMemory;  \
    }\
    return 0;\
}
memory_isValid(SkMemory);
memory_gc(SkMemory);

memory_isValid(SkMemoryFFFUI);
memory_gc(SkMemoryFFFUI);

const static luaL_Reg gSkMemory_Methods[] = {
        {"isValid", SkMemory_isValid},
        {"__gc", SkMemory_gc},
        {"NULL", NULL},
};
const static luaL_Reg gSkMemoryFFFUI_Methods[] = {
        {"isValid", SkMemoryFFFUI_isValid},
        {"__gc", SkMemoryFFFUI_gc},
        {"NULL", NULL},
};
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
    //register_bgfx(L);
    REG_CLASS(L, LuaApp);
    REG_CLASS(L, Init);
    REG_CLASS(L, PlatformData);
    REG_CLASS(L, Resolution);
    REG_CLASS(L, Init::Limits);
    REG_CLASS(L, Stats);
    REG_CLASS(L, VertexLayout);
    REG_CLASS(L, SkMemory);
    REG_CLASS(L, SkMemoryFFFUI);
    //TODO CallbackI*, bx::AllocatorI*
}

inline Init *getBgfxInit(lua_State *L) {
    return Bgfx_lua_app::requireInit(L);
}
// here we direct load bgfx_lua.
static const luaL_Reg bgfx_funcs[] = {
        {"getInit", bgfx_getInit},
        {"newApp", bgfx_newApp},
        {"setDebug", bgfx_setDebug},
        {"setViewClear", bgfx_setViewClear},
        {"setViewRect", bgfx_setViewRect},
        {"touch", bgfx_touch},
        {"dbgTextClear", bgfx_dbgTextClear},
        {"dbgTextImage", bgfx_dbgTextImage},
        {"dbgTextPrintf", bgfx_dbgTextPrintf},
        {"frame", bgfx_frame},
        {"getStats", bgfx_getStats},

        {"newVertexLayout", bgfx_newVertexLayout},
        {"newMemory", bgfx_newMemory},
        {"newMemoryFFFUI", bgfx_newMemoryFFFUI},
        {"makeRef", bgfx_makeRef},
        {"createVertexBuffer", bgfx_createVertexBuffer},
        {"createIndexBuffer", bgfx_createIndexBuffer},
        {"createProgram", bgfx_createProgram},

        //----- extra relative bgfx stand api ------
        {"loadProgram", bgfx_loadProgram},
        {nullptr, nullptr}
    };
extern "C" int luaopen_bgfx_lua(lua_State* L) {
    SkLua::Load(L);
    luaL_newlib(L, bgfx_funcs);
    return 1;
}
//--------------------------------------------------------
static int bx_getHPCounter(lua_State* L){
    auto i = bx::getHPCounter();
    lua_pushinteger(L, i);
    return 1;
}
static int bx_getHPFrequency(lua_State* L){
    auto i = bx::getHPFrequency();
    lua_pushinteger(L, i);
    return 1;
}
static const luaL_Reg bx_funcs[] = {
        {"getHPCounter", bx_getHPCounter},
        {"getHPFrequency", bx_getHPFrequency},
        {nullptr, nullptr}
};
extern "C" int luaopen_bx_lua(lua_State* L){
    luaL_newlib(L, bx_funcs);
    return 1;
}