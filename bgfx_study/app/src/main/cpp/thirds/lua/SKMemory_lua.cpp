//
// Created by Administrator on 2020/9/10 0010.
//
#include <cstring>
#include "SkMemory_lua.h"
#include "SkMemory.h"
#include "bgfx_wrapper.h"

static int _shouldWrapResult(lua_State *L, const char *names[]) {
    const char *func = luaL_checkstring(L, 1);
    int len = sizeof(names) / sizeof(names[0]);
    for (int i = 0; i < len; ++i) {
        if (strcmp(names[i], func) == 0) {
            lua_pushboolean(L, 1);
            return 1;
        }
    }
    return 0;
}

static int mem_new(lua_State *L) {
    //(type, table...) or (type, len )
    const char *type = lua_tostring(L, 1);
    int n = lua_gettop(L);
    auto secondType = lua_type(L, 2);
    if (secondType == LUA_TNUMBER) {
        //(type, len)
        int len = static_cast<int>(lua_tointeger(L, 2));
        lua_pop(L, 2);
        if (strlen(type) == 1) {
            SkMemory *pMemory = new SkMemory(type, len);
            LuaUtils::push_ptr(L, pMemory);
        } else {
            //multi types
            SkAnyMemory *pMemory = new SkAnyMemory(type, len);
            if (!pMemory->isValid()) {
                delete pMemory;
                return 0;
            }
            LuaUtils::push_ptr(L, pMemory);
        }
    } else if (secondType == LUA_TTABLE) {
        //(type, table...)
        lua_remove(L, 1);
        int tableCount = n - 1;
        if (strlen(type) == 1) {
            SkMemory *pMemory = new SkMemory(L, tableCount, type);
            LuaUtils::push_ptr(L, pMemory);
        } else {
            SkAnyMemory *pMemory = new SkAnyMemory(L, type);
            if (!pMemory->isValid()) {
                delete pMemory;
                return 0;
            }
            LuaUtils::push_ptr(L, pMemory);
        }
    } else {
        return luaL_error(L, "wrong arguments for Memory.new");
    }
    return 1;
}

static int mem_newFFFUI(lua_State *L) {
    //(table...)
    int tableCount = lua_gettop(L);
    SkMemoryFFFUI *pMemory = new SkMemoryFFFUI(L, tableCount);
    LuaUtils::push_ptr(L, pMemory);
    return 1;
}

static int mem_newMatrix(lua_State *L) {
    auto t = lua_tostring(L, 1);
    int luaType = lua_type(L, 2);
    if (luaType == LUA_TNUMBER) {
        int rowCount = static_cast<int>(lua_tointeger(L, 2));
        int columnCount = static_cast<int>(lua_tointeger(L, 3));
        SkMemoryMatrix *matrix = new SkMemoryMatrix(t, rowCount, columnCount);
        LuaUtils::push_ptr(L, matrix);
    } else if (luaType == LUA_TTABLE) {
        lua_remove(L, 1);
        SkMemoryMatrix *matrix = new SkMemoryMatrix(L, t);
        LuaUtils::push_ptr(L, matrix);
    } else {
        return luaL_error(L, "wrong arguments for create Memory Matrix.");
    }
    return 1;
}

static const luaL_Reg mem_funcs[] = {
        {"new",       mem_new},
        {"newFFFUI",  mem_newFFFUI},
        {"newMatrix", mem_newMatrix},
        {nullptr,     nullptr}
};
extern "C" int luaopen_hmem_lua(lua_State *L) {
    luaL_newlib(L, mem_funcs);
    return 1;
}

//------------------------------------------------------------
//---------------------- Memory --------------------------
#define memory_isValid(type)  \
static int type##_isValid(lua_State* L){ \
    auto pMemory = LuaUtils::get_ref<type>(L, 1); \
    lua_pushboolean(L, pMemory->_ref > 0); \
    return 1; \
}
#define memory_gc(type) \
static int type##_gc(lua_State* L){ \
    auto pMemory = LuaUtils::get_ref<type>(L, 1); \
    if(pMemory->unRef() == 0){ \
        pMemory->destroyData(); \
        delete pMemory;  \
    }\
    return 0;\
}
#define memory_len(type)  \
static int type##_len(lua_State* L){ \
    auto pMemory = LuaUtils::get_ref<type>(L, 1); \
    lua_pushinteger(L, pMemory->getLength()); \
    return 1; \
}
#define memory_tostring(type) \
static int type##_tostring(lua_State *L) {\
    auto pMemory = LuaUtils::get_ref<type>(L, 1); \
    const char* str = ((IMemory*)pMemory)->toString();\
    lua_pushstring(L, str);\
    free((void*)str);\
    return 1;\
}

#define memory_index(type) \
static int type##_index(lua_State *L) { \
    auto pMemory = LuaUtils::get_ref<type>(L, 1); \
    return type::read(pMemory, L); \
}

#define memory_newindex(type) \
static int type##_newindex(lua_State *L) { \
    auto pMemory = LuaUtils::get_ref<type>(L, 1); \
    return type::write(pMemory, L); \
}

#define memory_foreach(type) \
static int type##_foreach(lua_State *L) {\
    auto pMemory = LuaUtils::get_ref<type>(L, 1);\
    return pMemory->foreach(L);\
}

#define memory_shouldWrap(type) \
static int type##_shouldWrapResult(lua_State *L) {\
    const char* names[] = { \
            "convert", \
    }; \
    return _shouldWrapResult(L, names); \
}

#define memory_convert(type) \
static int type##_convert(lua_State *L) { \
    auto pMemory = LuaUtils::get_ref<type>(L, 1); \
    const char* types = luaL_checkstring(L, 2);\
    auto mem = pMemory->convert(types); \
    if(mem == nullptr){ \
        return luaL_error(L, "convert failed for types = '%s'", types); \
    } \
    SkMemory* skm = (SkMemory*)mem; \
    if(skm != nullptr){ \
        LuaUtils::push_ptr(L, skm); \
        return 1; \
    } \
    SkAnyMemory* skam = (SkAnyMemory*)mem; \
    if(skam != nullptr){ \
        LuaUtils::push_ptr(L, skam); \
        return 1; \
    } \
    return luaL_error(L, "can't reach here");\
}

memory_isValid(SkMemory)

memory_isValid(SkMemoryFFFUI)

memory_isValid(SkAnyMemory)

memory_gc(SkMemory)

memory_gc(SkMemoryFFFUI)

memory_gc(SkAnyMemory)

memory_len(SkMemory)

memory_len(SkMemoryFFFUI)

memory_len(SkAnyMemory)

memory_tostring(SkMemory)

memory_tostring(SkMemoryFFFUI)

memory_tostring(SkAnyMemory)

memory_index(SkMemory)

memory_index(SkMemoryFFFUI)

memory_index(SkAnyMemory)

memory_newindex(SkMemory)

memory_newindex(SkMemoryFFFUI)

memory_newindex(SkAnyMemory)

memory_foreach(SkMemory)

memory_foreach(SkMemoryFFFUI)

memory_foreach(SkAnyMemory)

memory_shouldWrap(SkMemory)

memory_shouldWrap(SkMemoryFFFUI)

memory_shouldWrap(SkAnyMemory)

memory_convert(SkMemory)
memory_convert(SkMemoryFFFUI)
memory_convert(SkAnyMemory)

const static luaL_Reg gSkAnyMemory_Methods[] = {
        {"convert",          SkAnyMemory_convert},
        {"foreach",          SkAnyMemory_foreach},
        {"isValid",          SkAnyMemory_isValid},
        {"_len",             SkAnyMemory_len},
        {"_tostring",        SkAnyMemory_tostring},
        {"_newindex",        SkAnyMemory_newindex},
        {"_index",           SkAnyMemory_index},

        {"call",             LuaUtils::forward_func_call},
        {"shouldWrapResult", SkAnyMemory_shouldWrapResult},
        {"__gc",             SkAnyMemory_gc},
        {NULL, NULL},
};
const static luaL_Reg gSkMemory_Methods[] = {
        {"convert",          SkMemory_convert},
        {"foreach",          SkMemory_foreach},
        {"isValid",          SkMemory_isValid},
        {"_len",             SkMemory_len},
        {"_tostring",        SkMemory_tostring},
        {"_newindex",        SkMemory_newindex},
        {"_index",           SkMemory_index},

        {"call",             LuaUtils::forward_func_call},
        {"shouldWrapResult", SkMemory_shouldWrapResult},
        {"__gc",             SkMemory_gc},
        {NULL, NULL},
};
const static luaL_Reg gSkMemoryFFFUI_Methods[] = {
        {"convert",          SkMemoryFFFUI_convert},
        {"foreach",          SkMemoryFFFUI_foreach},
        {"isValid",          SkMemoryFFFUI_isValid},
        {"_len",             SkMemoryFFFUI_len},
        {"_tostring",        SkMemoryFFFUI_tostring},
        {"_newindex",        SkMemoryFFFUI_newindex},
        {"_index",           SkMemoryFFFUI_index},

        {"call",             LuaUtils::forward_func_call},
        {"shouldWrapResult", SkMemoryFFFUI_shouldWrapResult},
        {"__gc",             SkMemoryFFFUI_gc},
        {NULL, NULL},
};
#define PUSH_PTR(type) \
void type##_push(lua_State* L, type* ptr){ \
    LuaUtils::push_ptr<type>(L, ptr); \
}
#define PULL_PTR(type) \
type* type##_pull(lua_State* L,int index){ \
    return LuaUtils::get_ref<type>(L, index); \
}

PUSH_PTR(SkMemory)

PULL_PTR(SkMemory)

PUSH_PTR(SkAnyMemory)

PULL_PTR(SkAnyMemory)

static int SkMemoryMatrix_index(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, 1);
    if (pMemory->isSingleType()) {
        return SkMemoryMatrix::read(pMemory, L, SkMemory_push);
    }
    return SkMemoryMatrix::read(pMemory, L, SkAnyMemory_push);
}

static int SkMemoryMatrix_newindex(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, 1);
    if (pMemory->isSingleType()) {
        return SkMemoryMatrix::write(pMemory, L, SkMemory_pull);
    } else {
        return SkMemoryMatrix::write(pMemory, L, SkAnyMemory_pull);
    }
}

static int SkMemoryMatrix_gc(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, 1);
    delete pMemory;
    return 0;
}

static int SkMemoryMatrix_isValid(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, 1);
    lua_pushboolean(L, pMemory->isValid() ? 1 : 0);
    return 1;
}

static int SkMemoryMatrix_len(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, 1);
    lua_pushinteger(L, pMemory->getRowCount());
    return 1;
}

static int SkMemoryMatrix_columnCount(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, 1);
    lua_pushinteger(L, pMemory->getColumnCount());
    return 1;
}

static int SkMemoryMatrix_tostring(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, 1);
    MEM_PUSH_TO_STRING(L, pMemory);
    return 1;
}

static int SkMemoryMatrix_transpose(lua_State *L) {
    auto mat = LuaUtils::get_ref<SkMemoryMatrix>(L, 1);
    auto pMatrix = mat->transpose();
    if (pMatrix == nullptr) {
        return luaL_error(L,
                          "this matrix doesn't support transpose. please convert data type first");
    }
    LuaUtils::push_ptr(L, pMatrix);
    return 1;
}

static int SkMemoryMatrix_convert(lua_State *L) {
    //mat, types
    auto mat = LuaUtils::get_ref<SkMemoryMatrix>(L, 1);
    auto types = luaL_checkstring(L, 2);
    auto pMatrix = mat->convert(types);
    if (pMatrix == nullptr) {
        return luaL_error(L, "this matrix can't convert to types = '%s'", types);
    }
    LuaUtils::push_ptr(L, pMatrix);
    return 1;
}

static int SkMemoryMatrix_shouldWrapResult(lua_State *L) {
    const char *names[] = {
            "transpose",
            "convert",
    };
    return _shouldWrapResult(L, names);
}

const static luaL_Reg gSkMemoryMatrix_Methods[] = {
        {"convert",          SkMemoryMatrix_convert},
        {"transpose",        SkMemoryMatrix_transpose},
        {"isValid",          SkMemoryMatrix_isValid},
        {"getColumnCount",   SkMemoryMatrix_columnCount},
        {"getRowCount",      SkMemoryMatrix_len},
        {"_len",             SkMemoryMatrix_len},
        {"_tostring",        SkMemoryMatrix_tostring},
        {"_newindex",        SkMemoryMatrix_newindex},
        {"_index",           SkMemoryMatrix_index},

        {"call",             LuaUtils::forward_func_call},
        {"shouldWrapResult", SkMemoryMatrix_shouldWrapResult},
        {"__gc",             SkMemoryMatrix_gc},
        {NULL, NULL},
};

DEF_MTNAME(SkMemory)

DEF_MTNAME(SkMemoryFFFUI)

DEF_MTNAME(SkAnyMemory)

DEF_MTNAME(SkMemoryMatrix)

void SkMemoryLua::registers(lua_State *L) {
    REG_CLASS(L, SkMemory);
    REG_CLASS(L, SkMemoryFFFUI);
    REG_CLASS(L, SkAnyMemory);
    REG_CLASS(L, SkMemoryMatrix);
}