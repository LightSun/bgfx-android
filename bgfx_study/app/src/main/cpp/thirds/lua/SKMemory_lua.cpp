//
// Created by Administrator on 2020/9/10 0010.
//
#include <cstring>
#include "SkMemory_lua.h"
#include "SkMemory.h"
#include "bgfx_wrapper.h"
#include "log.h"

//----------------------------------------------------------------

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
            LuaUtils::push_ptr(L, pMemory);
        }
    } else {
        return luaL_error(L, "wrong arguments for Memory.new");
    }
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

#ifdef H_INTERNAL_TEST
static int mem_test_comp(lua_State *L) {
    auto str1 = lua_tostring(L, 1);
    auto str2 = lua_tostring(L, 2);
    auto type = MemoryUtils::computeType(MemoryUtils::computeType(str1),
            MemoryUtils::computeType(str2));
    char res[2];
    res[0] = type;
    res[1] = '\0';
    lua_pushstring(L, res);
    return 1;
}
#endif

static const luaL_Reg mem_funcs[] = {
        {"new",       mem_new},
        {"newMat",    mem_newMatrix},
#ifdef H_INTERNAL_TEST
        {"test_comp", mem_test_comp},
#endif
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
    auto pMemory = LuaUtils::get_ref<type>(L, lua_upvalueindex(1)); \
    lua_pushboolean(L, pMemory->_ref > 0); \
    return 1; \
}
#define memory_gc(type) \
static int type##_gc(lua_State* L){ \
    auto pMemory = LuaUtils::get_ref<type>(L, 1); \
    pMemory->unRefAndDestroy();\
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

//tab, index ,val
#define memory_newindex(type) \
static int type##_newindex(lua_State *L) { \
    auto pMemory = LuaUtils::get_ref<type>(L, 1); \
    pMemory->setValue(lua_tointeger(L, -2), lua_tonumber(L, -1));\
    return 0;\
}

#define memory_foreach(type) \
static int type##_foreach(lua_State *L) {\
    auto pMemory = LuaUtils::get_ref<type>(L, lua_upvalueindex(1));\
    return pMemory->foreach(L);\
}

#define memory_convert(type) \
static int type##_convert(lua_State *L) { \
    auto pMemory = LuaUtils::get_ref<type>(L, lua_upvalueindex(1)); \
    const char* types = luaL_checkstring(L, -1);\
    return pMemory->convert(L, types); \
}
#define memory_copy(type) \
static int type##_copy(lua_State *L) { \
    auto pMemory = LuaUtils::get_ref<type>(L, lua_upvalueindex(1)); \
    auto sr = pMemory->copy(); \
    if(sr != NULL){ \
        LuaUtils::push_ptr(L, sr); \
        return 1; \
    } \
    return 0; \
}
#define memory_GET_TYPES(mem) \
static int mem##_getTypes(lua_State *L) { \
auto pMemory = LuaUtils::get_ref<mem>(L, lua_upvalueindex(1)); \
lua_pushstring(L, pMemory->getTypes()); \
return 1; \
}

#define memory_extract(mem) \
static int mem##_extract(lua_State *L) { \
auto pMemory = LuaUtils::get_ref<mem>(L, lua_upvalueindex(1)); \
SkMemory *outMem; \
if(lua_gettop(L) == 1){ \
    outMem = pMemory->extract(lua_tointeger(L, -1), pMemory->getLength());\
}else{\
    outMem = pMemory->extract(lua_tointeger(L, -2), lua_tointeger(L, -1));\
}\
if(outMem == NULL){ \
    return 0; \
} \
LuaUtils::push_ptr(L, outMem);\
return 1; \
}

#define memory_kickOut(mem) \
static int mem##_kickOut(lua_State *L) { \
auto pMemory = LuaUtils::get_ref<mem>(L, lua_upvalueindex(1)); \
SkMemory *outMem = pMemory->kickOut(lua_tointeger(L, -1)); \
if(outMem == NULL){ \
    return 0; \
} \
LuaUtils::push_ptr(L, outMem);\
return 1; \
}

#define memory_eq(mem) \
static int mem##_eq(lua_State *L) { \
auto mem1 = LuaUtils::to_ref<mem>(L, 1); \
auto mem2 = LuaUtils::to_ref<mem>(L, 2); \
auto eq = mem1 !=NULL ? mem1->equals(mem2) :(mem2 == NULL);\
lua_pushboolean(L, eq);\
return 1;\
}

//count, type, def value
#define memory_reshape(mem, method)\
static int mem##_##method(lua_State *L) {\
    auto pMemory = LuaUtils::get_ref<mem>(L, lua_upvalueindex(1));\
    auto count = lua_tointeger(L, 1);\
    if(count <= 0 ){ \
        return luaL_error(L, "wrong count for reshape.");\
    }\
    auto top = lua_gettop(L);\
    SkMemory* result = nullptr;\
    switch (top){\
        case 3:{\
            auto str = lua_tostring(L, 2);\
            result = pMemory->method(count, str[0], lua_tonumber(L, 3));\
        }break;\
        case 2:{\
            auto lt = lua_type(L, 2);\
            if(lt == LUA_TNUMBER){\
                result = pMemory->method(count, DEF_RESHAPE_TYPE, lua_tonumber(L, 2));\
            } else if(lt == LUA_TSTRING){\
                auto str = lua_tostring(L, 2);\
                result = pMemory->method(count, str[0]);\
            } else{\
                return luaL_error(L, "wrong arguments. expect is (count, [type, default_value])");\
            }\
        }break;\
        case 1:{\
            result = pMemory->method(count);\
        }break;\
        default:\
            return luaL_error(L, "wrong arguments. expect is (count, [type, default_value])");\
    }\
    if(result != nullptr){\
        LuaUtils::push_ptr(L, result);\
        return 1;\
    }\
    return 0;\
}

#define memory_concat(mem)\
static int mem##_concat(lua_State *L) {\
    auto pMemory = LuaUtils::get_ref<mem>(L, lua_upvalueindex(1));\
    const auto msg = "wrong arguments , expect is (mem, resultCount[,type, default_value])";\
    SkMemory* oth = LuaUtils::to_ref<SkMemory>(L, 1);\
    SkAnyMemory* oth_any = nullptr;\
    if(oth == nullptr){\
        oth_any = LuaUtils::to_ref<SkAnyMemory>(L, 1);\
        if(oth_any == nullptr){\
            return luaL_error(L, msg);\
        }\
    }\
    auto top = lua_gettop(L);\
    SkMemory * result = nullptr;\
    switch(top){\
        case 4:{\
            auto count = lua_tointeger(L, 2);\
            auto str = lua_tostring(L, 3);\
            if(oth != nullptr){\
                result = pMemory->concat(oth, count, str[0], lua_tonumber(L, 4));\
            } else{\
                result = pMemory->concat(oth_any, count, str[0], lua_tonumber(L, 4));\
            }\
        }break;\
        case 3:{\
            auto type2 = lua_type(L, 2);\
            if(type2 == LUA_TSTRING){\
                auto str = lua_tostring(L, 2);\
                if(oth != nullptr){\
                    int c = pMemory->getLength() + oth->getLength();\
                    result = pMemory->concat(oth, c, str[0], lua_tonumber(L, 3));\
                } else{\
                    int c = pMemory->getLength() + oth_any->getLength();\
                    result = pMemory->concat(oth_any, c, str[0], lua_tonumber(L, 3));\
                }\
            } else if(type2 == LUA_TNUMBER){\
                switch(lua_type(L, 3)){\
                    case LUA_TNUMBER:\
                        if(oth != nullptr){\
                            result = pMemory->concat(oth, lua_tointeger(L, 2), DEF_RESHAPE_TYPE, lua_tonumber(L, 3));\
                        } else{\
                            result = pMemory->concat(oth_any, lua_tointeger(L, 2), DEF_RESHAPE_TYPE, lua_tonumber(L, 3));\
                        }\
                        break;\
                    case LUA_TSTRING:\
                        auto str = lua_tostring(L, 3);\
                        if(oth != nullptr){\
                            result = pMemory->concat(oth, lua_tointeger(L, 2), str[0]);\
                        } else{\
                            result = pMemory->concat(oth_any, lua_tointeger(L, 2), str[0]);\
                        }\
                        break;\
                }\
            } else{\
                return luaL_error(L, msg);\
            }\
        }break;\
        case 2:{\
            if(oth != nullptr){\
                result = pMemory->concat(oth, lua_tointeger(L, 2));\
            } else{\
                result = pMemory->concat(oth_any, lua_tointeger(L, 2));\
            }\
        }break;\
        case 1:{\
            if(oth != nullptr){\
                int c = pMemory->getLength() + oth->getLength();\
                result = pMemory->concat(oth, c);\
            } else{\
                int c = pMemory->getLength() + oth_any->getLength();\
                result = pMemory->concat(oth_any, c);\
            }\
        }break;\
    }\
    if(result != nullptr){\
        LuaUtils::push_ptr(L, result);\
        return 1;\
    }\
    return luaL_error(L, msg);\
}

#define memory_diag(mem)\
static int mem##_diag(lua_State *L) {\
    auto pMemory = LuaUtils::get_ref<mem>(L, lua_upvalueindex(1));\
    SkMemoryMatrix * result = nullptr;\
    switch(lua_gettop(L)){\
        case 3:{\
            auto str = lua_tostring(L, 2);\
            result = pMemory->diag(lua_tointeger(L, 1), str[0], lua_tonumber(L, 3));\
        }break;\
        case 2:{\
            int t = lua_type(L, 2);\
            if(t == LUA_TSTRING){\
                auto str = lua_tostring(L, 2);\
                result = pMemory->diag(lua_tointeger(L, 1), str[0]);\
            } else if(t == LUA_TNUMBER){\
                result = pMemory->diag(lua_tointeger(L, 1), DEF_RESHAPE_TYPE, lua_tonumber(L, 2));\
            }\
        }break;\
        case 1:{\
            int t = lua_type(L, 1);\
            if(t == LUA_TSTRING){\
                 auto str = lua_tostring(L, 1);\
                 result = pMemory->diag(0, str[0]);\
            }else if(t == LUA_TNUMBER){\
                 result = pMemory->diag(lua_tointeger(L, 1));\
            }\
        }break;\
        case 0:{\
            result = pMemory->diag();\
        }break;\
    }\
    if(result != nullptr){\
        LuaUtils::push_ptr(L, result);\
        return 1;\
    }\
    return luaL_error(L, "for 'memory.diag' arguments expect([int k, int default_val])");\
}
memory_diag(SkMemory)
memory_diag(SkAnyMemory)

memory_reshape(SkMemory,reshape)
memory_reshape(SkAnyMemory,reshape)
memory_reshape(SkMemory,reshapeBefore)
memory_reshape(SkAnyMemory,reshapeBefore)

memory_concat(SkMemory)
memory_concat(SkAnyMemory)

memory_eq(SkMemory)
memory_eq(SkAnyMemory)
memory_eq(SkMemoryMatrix)

memory_copy(SkMemory)
memory_copy(SkAnyMemory)

memory_isValid(SkMemory)
memory_isValid(SkAnyMemory)

memory_gc(SkMemory)
memory_gc(SkAnyMemory)

memory_len(SkMemory)
memory_len(SkAnyMemory)

memory_tostring(SkMemory)
memory_tostring(SkAnyMemory)

memory_newindex(SkMemory)
memory_newindex(SkAnyMemory)

memory_foreach(SkMemory)
memory_foreach(SkAnyMemory)

memory_convert(SkMemory)
memory_convert(SkAnyMemory)

memory_GET_TYPES(SkMemory)
memory_GET_TYPES(SkAnyMemory)
memory_GET_TYPES(SkMemoryMatrix)

memory_extract(SkMemory)
memory_extract(SkAnyMemory)

memory_kickOut(SkMemory)
memory_kickOut(SkAnyMemory)

static int SkMemory_flip(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemory>(L, lua_upvalueindex(1));
    SkMemory* result = nullptr;
    if(lua_gettop(L) > 0){
        result = pMemory->flip(lua_toboolean(L, -1) == 1);
    } else{
        result = pMemory->flip();
    }
    LuaUtils::push_ptr(L, result);
    return 1;
}
static int SkAnyMemory_flip(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkAnyMemory>(L, lua_upvalueindex(1));
    LuaUtils::push_ptr(L, pMemory->flip());
    return 1;
}

#define DEF_V_METHODS(type) \
static const luaL_Reg s##type##_Methods[] = { \
        {"getTypes",         type##_getTypes}, \
        {"copy",             type##_copy}, \
        {"convert",          type##_convert}, \
        {"foreach",          type##_foreach},\
        {"isValid",          type##_isValid}, \
        {"extract",          type##_extract}, \
        {"kickOut",          type##_kickOut}, \
        {"reshape",          type##_reshape}, \
        {"reshapeBefore",    type##_reshapeBefore}, \
        {"concat",           type##_concat}, \
        {"flip",             type##_flip}, \
        {"diag",             type##_diag}, \
        {nullptr,            nullptr}, \
};
DEF_V_METHODS(SkMemory)
DEF_V_METHODS(SkAnyMemory)

//table, index // old is return type::read(pMemory, L);
#define memory_index(type) \
static int type##_index(lua_State *L) { \
    auto pMemory = LuaUtils::get_ref<type>(L, 1); \
    if(lua_type(L, 2) == LUA_TNUMBER){ \
        double val; \
        if(pMemory->getValue(lua_tointeger(L, -1), &val)){ \
            lua_pushnumber(L, val); \
            return 1; \
        }else{ \
            return 0;\
        } \
    } else{ \
        auto str = luaL_checkstring(L, 2); \
        auto methods = s##type##_Methods; \
        for (int i = 0; ; ++i) { \
            if(methods[i].name == nullptr){ \
                break; \
            } \
            if(strcmp(methods[i].name, str) == 0){ \
                lua_pushvalue(L, 1); \
                lua_pushcclosure(L, methods[i].func, 1); \
                return 1; \
            } \
        } \
        lua_pushnil(L); \
        return 1; \
    } \
}

memory_index(SkMemory)
memory_index(SkAnyMemory)

//mat, val
#define memory_mul(type, type_str) \
static int type##_mul(lua_State *L) { \
    auto pMemory = LuaUtils::get_ref<type>(L, 1);\
    switch(lua_type(L, 2)){ \
        case LUA_TNUMBER: { \
            auto pMatrix = pMemory->_mul(lua_tonumber(L, 2)); \
            LuaUtils::push_ptr(L, pMatrix); \
            return 1; \
        } \
        default:{ \
            auto skm = LuaUtils::to_ref<SkMemory>(L, 2); \
            if(skm != nullptr){ \
                LOGD("multile SkMemory"); \
                auto pMatrix = pMemory->_mul(skm); \
                if(pMatrix == nullptr){ \
                    return luaL_error(L, "for %s multiple simple-array(SkMemory). matrix.column must be 1.",  type_str); \
                } \
                LuaUtils::push_ptr(L, pMatrix); \
                return 1; \
            } \
            auto skam = LuaUtils::to_ref<SkAnyMemory>(L, 2); \
            if(skam != nullptr){  \
                LOGD("multile SkAnyMemory"); \
                auto pMatrix = pMemory->_mul(skam); \
                if(pMatrix == nullptr){ \
                    return luaL_error(L, "for %s multiple array(SkAnyMemory). matrix.column must be 1.", type_str); \
                } \
                LuaUtils::push_ptr(L, pMatrix);\
                return 1; \
            } \
            auto mat = LuaUtils::to_ref<SkMemoryMatrix>(L, 2); \
            if(mat != nullptr){\
                  LOGD("multile SkMemoryMatrix"); \
                auto pMatrix = pMemory->_mul(mat);\
                if(pMatrix == nullptr){\
                    return luaL_error(L, "for %s multiple mat. mat1.columnCount must equals mat2.rowCount.", type_str);\
                }\
                LuaUtils::push_ptr(L, pMatrix);\
                return 1; \
            }   \
            return luaL_error(L, "wrong data type of index 2"); \
        } \
    } \
}
memory_mul(SkMemoryMatrix, "mat")
memory_mul(SkMemory, "simple-array")
memory_mul(SkAnyMemory, "array")

const static luaL_Reg gSkAnyMemory_Methods[] = {
        {"__mul",             SkAnyMemory_mul},
        {"__eq",              SkAnyMemory_eq},
        {"__len",             SkAnyMemory_len},
        {"__tostring",        SkAnyMemory_tostring},
        {"__newindex",        SkAnyMemory_newindex},
        {"__index",           SkAnyMemory_index},
        {"__gc",             SkAnyMemory_gc},
        {NULL, NULL},
};
const static luaL_Reg gSkMemory_Methods[] = {
        {"__mul",             SkMemory_mul},
        {"__eq",              SkMemory_eq},
        {"__len",             SkMemory_len},
        {"__tostring",        SkMemory_tostring},
        {"__newindex",        SkMemory_newindex},
        {"__index",           SkMemory_index},
        {"__gc",             SkMemory_gc},
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

static int SkMemoryMatrix_len(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, 1);
    lua_pushinteger(L, pMemory->getRowCount());
    return 1;
}
//push string and then release
#define MEM_PUSH_TO_STRING(L, ptr) \
const char* str = ((IMemory*)ptr)->toString(); \
lua_pushstring(L, str);\
free((void*)str);

static int SkMemoryMatrix_tostring(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, 1);
    MEM_PUSH_TO_STRING(L, pMemory);
    return 1;
}

static int SkMemoryMatrix_isValid(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, lua_upvalueindex(1));
    lua_pushboolean(L, pMemory->isValid() ? 1 : 0);
    return 1;
}

static int SkMemoryMatrix_columnCount(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, lua_upvalueindex(1));
    lua_pushinteger(L, pMemory->getColumnCount());
    return 1;
}

static int SkMemoryMatrix_transpose(lua_State *L) {
    auto mat = LuaUtils::get_ref<SkMemoryMatrix>(L, lua_upvalueindex(1));
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
    auto mat = LuaUtils::get_ref<SkMemoryMatrix>(L, lua_upvalueindex(1));
    auto types = luaL_checkstring(L, -1);
    return mat->convert(L , types);
}

static int SkMemoryMatrix_getRowCount(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, lua_upvalueindex(1));
    lua_pushinteger(L, pMemory->getRowCount());
    return 1;
}
static int SkMemoryMatrix_inverse(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, lua_upvalueindex(1));
    return pMemory->inverse(L);
}

static int SkMemoryMatrix_determinant(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, lua_upvalueindex(1));
    if(pMemory->isSquareMatrix()){
        lua_pushnumber(L, pMemory->determinant());
        return 1;
    }
    return 0;
}
static int SkMemoryMatrix_remainderValue(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, lua_upvalueindex(1));
    if(pMemory->isSquareMatrix()){
        auto val = pMemory->remainderValue(lua_tointeger(L, -2), lua_tointeger(L, -1));
        lua_pushnumber(L, val);
        return 1;
    }
    return 0;
}
static int SkMemoryMatrix_remainderMat(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, lua_upvalueindex(1));
    auto val = pMemory->remainderMat(lua_tointeger(L, -2), lua_tointeger(L, -1));
    if(val != NULL){
        LuaUtils::push_ptr(L, val);
        return 1;
    }
    return 0;
}
static int SkMemoryMatrix_algebraicRemainderMat(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, lua_upvalueindex(1));
    auto val = pMemory->algebraicRemainderMat();
    if(val != NULL){
        LuaUtils::push_ptr(L, val);
        return 1;
    }
    return 0;
}
static int SkMemoryMatrix_adjointMat(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, lua_upvalueindex(1));
    auto val = pMemory->adjointMat();
    if(val != NULL){
        LuaUtils::push_ptr(L, val);
        return 1;
    }
    return 0;
}
static int SkMemoryMatrix_extractCol(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, lua_upvalueindex(1));
    auto outMem = pMemory->extractColumn(lua_tointeger(L, -1), NULL);
    if(outMem != nullptr){
        LuaUtils::push_ptr(L, outMem);
        return 1;
    }
    return 0;
}
static int SkMemoryMatrix_extractMat(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, lua_upvalueindex(1));
    int c = lua_gettop(L);
    SkMemoryMatrix * result = nullptr;
    switch(c){
        case 4:
            result = pMemory->extractMat(lua_tointeger(L,-4), lua_tointeger(L,-3),
                                         lua_tointeger(L,-2), lua_tointeger(L,-1));
            break;
        case 3:
            result = pMemory->extractMat(lua_tointeger(L,-3), lua_tointeger(L,-2),
                                         lua_tointeger(L,-1), pMemory->getColumnCount());
            break;
        case 2:
            result = pMemory->extractMat(lua_tointeger(L,-2), pMemory->getRowCount(),
                                         lua_tointeger(L,-1), pMemory->getColumnCount());
            break;
        case 1:
            result = pMemory->extractMat(lua_tointeger(L,-1), pMemory->getRowCount(),
                                         0, pMemory->getColumnCount());
            break;
        default:
            return luaL_error(L, "wrong arguments for 'SkMemoryMatrix->extractMat(...)'");

    }
    if(result != NULL){
        LuaUtils::push_ptr(L, result);
        return 1;
    }
    return 0;
}
static int SkMemoryMatrix_reshape(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, lua_upvalueindex(1));
    int c = lua_gettop(L);
    SkMemoryMatrix* result;
    switch (c){
        case 4: {
            auto str = lua_tostring(L, -2);
            result = pMemory->reshape(lua_tointeger(L, -4), lua_tointeger(L, -3),
                                      str[0], lua_tonumber(L, -1));
        }
            break;
        case 3:{
            if(lua_type(L, -1) == LUA_TSTRING){
                auto str = lua_tostring(L, -1);
                result = pMemory->reshape(lua_tointeger(L, -3), lua_tointeger(L, -2),
                                          str[0]);
            } else if(lua_type(L, -1) == LUA_TNUMBER){
                result = pMemory->reshape(lua_tointeger(L, -3), lua_tointeger(L, -2), DEF_RESHAPE_TYPE,
                                          lua_tonumber(L, -1));
            } else{
                return luaL_error(L, "wrong arguments for mat.reshape. expect is (int, int, string, number)");
            }
        }
            break;
        default:
            return luaL_error(L, "wrong arguments for mat.reshape. expect is (int, int, string, number)");
    }
    if(result != NULL){
        LuaUtils::push_ptr(L, result);
        return 1;
    }
    return 0;
}
static int SkMemoryMatrix_concat(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, lua_upvalueindex(1));
    int c = lua_gettop(L);
    SkMemoryMatrix* result;
    switch (c){
        case 3:{
            result = pMemory->concat(LuaUtils::get_ref<SkMemoryMatrix>(L, -3),
                    lua_toboolean(L, -2) != 0, lua_tonumber(L, -1));
        }break;

        case 2:{
            if(lua_type(L, -1) == LUA_TBOOLEAN){
                result = pMemory->concat(LuaUtils::get_ref<SkMemoryMatrix>(L, -2), lua_toboolean(L, -1) != 0);
            } else{
                result = pMemory->concat(LuaUtils::get_ref<SkMemoryMatrix>(L, -2), true, lua_tonumber(L, -1));
            }
        }break;

        case 1:{
            result = pMemory->concat(LuaUtils::get_ref<SkMemoryMatrix>(L, -1));
        }break;

        default:
            return luaL_error(L, "wrong arguments for mat.concat. expect is (int, int, string, number)");
    }
    if(result != NULL){
        LuaUtils::push_ptr(L, result);
        return 1;
    }
    return 0;
}
static int SkMemoryMatrix_diag(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, lua_upvalueindex(1));
    SkMemory* result = nullptr;
    if(lua_gettop(L) == 1){
        result = pMemory->diag(lua_tointeger(L, 1));
    } else{
        result = pMemory->diag();
    }
    if(result != NULL){
        LuaUtils::push_ptr(L, result);
        return 1;
    }
    return 0;
}
//fliplr, flipud, triu, tril
#define SkMemoryMatrix_flip(method)\
static int SkMemoryMatrix_##method(lua_State *L) {\
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, lua_upvalueindex(1));\
    SkMemoryMatrix* result = nullptr;\
    if(lua_gettop(L) == 1){\
        result = pMemory->method(lua_toboolean(L, 1) == 1);\
    } else{\
        result = pMemory->method();\
    }\
    if(result != NULL){\
        LuaUtils::push_ptr(L, result);\
        return 1;\
    }\
    return 0;\
}
#define SkMemoryMatrix_triul(method)\
static int SkMemoryMatrix_##method(lua_State *L) {\
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, lua_upvalueindex(1));\
    SkMemoryMatrix* result = nullptr;\
    switch (lua_gettop(L)){\
        case 2:{\
            result = pMemory->method(lua_tointeger(L, 1), lua_tonumber(L, 2));\
        }break;\
        case 1:{\
            result = pMemory->method(lua_tointeger(L, 1));\
        }break;\
        case 0:{\
            result = pMemory->method();\
        }break;\
    }\
    if(result != NULL){\
        LuaUtils::push_ptr(L, result);\
        return 1;\
    }\
    return 0;\
}
SkMemoryMatrix_triul(triu)
SkMemoryMatrix_triul(tril)

SkMemoryMatrix_flip(fliplr)
SkMemoryMatrix_flip(flipud)

memory_copy(SkMemoryMatrix)
const static luaL_Reg sSkMemoryMatrix_Methods[] = {
        {"getTypes",         SkMemoryMatrix_getTypes},
        {"copy",             SkMemoryMatrix_copy},
        {"convert",          SkMemoryMatrix_convert},
        {"transpose",        SkMemoryMatrix_transpose},
        {"isValid",          SkMemoryMatrix_isValid},
        {"getColumnCount",   SkMemoryMatrix_columnCount},
        {"getRowCount",      SkMemoryMatrix_getRowCount},
        {"extractMat",       SkMemoryMatrix_extractMat},
        {"extractCol",       SkMemoryMatrix_extractCol},
        {"inverse",          SkMemoryMatrix_inverse},

        {"reshape",          SkMemoryMatrix_reshape},
        {"concat",           SkMemoryMatrix_concat},
        {"diag",             SkMemoryMatrix_diag},
        {"fliplr",           SkMemoryMatrix_fliplr},
        {"flipud",           SkMemoryMatrix_flipud},
        {"triu",             SkMemoryMatrix_triu},
        {"tril",             SkMemoryMatrix_tril},

        {"determinant",       SkMemoryMatrix_determinant},
        {"remainderValue",    SkMemoryMatrix_remainderValue},
        {"remainderMat",      SkMemoryMatrix_remainderMat},
        {"algRemainderMat",   SkMemoryMatrix_algebraicRemainderMat},
        {"adjointMat",        SkMemoryMatrix_adjointMat},
        {NULL, NULL},
};

static int SkMemoryMatrix_index(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, 1);
    //id index
    if(lua_type(L, 2) == LUA_TNUMBER){
        if (pMemory->isSingleType()) {
            return SkMemoryMatrix::read(pMemory, L, SkMemory_push);
        }
        return SkMemoryMatrix::read(pMemory, L, SkAnyMemory_push);
    } else{
        //method
        const auto mn = luaL_checkstring(L, 2);
        for (int i = 0; ; ++i) {
            auto method = sSkMemoryMatrix_Methods[i];
            if(method.name == nullptr){
                break;
            }
            if(strcmp(method.name, mn) == 0){
                lua_pushvalue(L, 1);
                lua_pushcclosure(L, method.func, 1);
                return 1;
            }
        }
        lua_pushnil(L);
        return 1;
    }
}

static int SkMemoryMatrix_newindex(lua_State *L) {
    auto pMemory = LuaUtils::get_ref<SkMemoryMatrix>(L, 1);
    if (pMemory->isSingleType()) {
        return SkMemoryMatrix::write(pMemory, L, SkMemory_pull);
    } else {
        return SkMemoryMatrix::write(pMemory, L, SkAnyMemory_pull);
    }
}
memory_gc(SkMemoryMatrix)
const static luaL_Reg gSkMemoryMatrix_Methods[] = {
        {"__mul",             SkMemoryMatrix_mul},
        {"__eq",              SkMemoryMatrix_eq},
        {"__len",             SkMemoryMatrix_len},
        {"__tostring",        SkMemoryMatrix_tostring},
        {"__newindex",        SkMemoryMatrix_newindex},
        {"__index",           SkMemoryMatrix_index},
        {"__gc",              SkMemoryMatrix_gc},
        {NULL, NULL},
};

DEF_MTNAME(SkMemory)

DEF_MTNAME(SkAnyMemory)

DEF_MTNAME(SkMemoryMatrix)

void SkMemoryLua::registers(lua_State *L) {
    REG_CLASS(L, SkMemory);
    REG_CLASS(L, SkAnyMemory);
    REG_CLASS(L, SkMemoryMatrix);
}