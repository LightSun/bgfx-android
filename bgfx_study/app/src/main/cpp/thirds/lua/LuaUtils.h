//
// Created by Administrator on 2020/8/28 0028.
//

#ifndef BGFX_STUDY_LUAUTILS_H
#define BGFX_STUDY_LUAUTILS_H

#include "SkRefCnt.h"
#include "SkScalar.h"
#include "lua.hpp"
#include "global.h"

#define lua2bool(L, idx) lua_toboolean(L, idx) == 1
#define SkScalarToLua(x)    SkScalarToDouble(x)
#define SkLuaToScalar(x)    SkDoubleToScalar(x)

#define TO_NUMBER_8(L, idx) static_cast<uint8_t>(lua_tonumber(L, idx))
#define TO_NUMBER_16(L, idx) static_cast<uint16_t>(lua_tonumber(L, idx))
#define TO_NUMBER_32(L, idx) static_cast<uint32_t>(lua_tonumber(L, idx))
#define TO_FLOAT(L, idx) static_cast<float>(lua_tonumber(L, idx))
#define TO_INT(L, idx) static_cast<int>(lua_tonumber(L, idx))
#define TO_UINT(L, idx) static_cast<unsigned int>(lua_tonumber(L, idx))
#define TO_SHORT(L, idx) static_cast<short>(lua_tonumber(L, idx))
#define TO_CHAR(L, idx) static_cast<s_char>(lua_tonumber(L, idx))
#define TO_DOUBLE(L, idx) static_cast<double>(lua_tonumber(L, idx))

#define LUA_OPT(L, idx, func_idx, defVal) \
if(lua_type(L, idx) != nullptr){ \
    return func_idx(L, idx)); \
} \
return defVal;

#define OPT_Number_8(L, idx, defVal) LUA_OPT(L, idx, TO_NUMBER_8, defVal)


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

#define CHECK_SETFIELD(key) do if (key) lua_setfield(fL, -2, key); while (0)

#define DEF__INDEX_NUM(type, func_num)        \
static int type##_index(lua_State *L) { \
    auto pMemory = LuaUtils::get_ref<type>(L, 1); \
    if(lua_type(L, 2) == LUA_TNUMBER){ \
        return func_num(L, pMemory); \
    } else{ \
        const auto mn = luaL_checkstring(L, 2);\
        for (int i = 0; ; ++i) {\
            auto method = s##type##_Methods[i];\
            if(method.name == nullptr){\
                break;\
            }\
            if(strcmp(method.name, mn) == 0){\
                lua_pushvalue(L, 1);\
                lua_pushcclosure(L, method.func, 1);\
                return 1;\
            }\
        }\
        lua_pushnil(L);\
        return 1;\
    }\
}

#define DEF__INDEX(m_prefix, type)        \
static int m_prefix##_index(lua_State *L) { \
    auto pMemory = LuaUtils::get_ref<type>(L, 1); \
    const auto mn = luaL_checkstring(L, 2);\
    for (int i = 0; ; ++i) {\
        auto method = s##type##_Methods[i];\
        if(method.name == nullptr){\
            break;\
        }\
        if(strcmp(method.name, mn) == 0){\
            lua_pushvalue(L, 1);\
            lua_pushcclosure(L, method.func, 1);\
            return 1;\
        }\
    }\
    return 0;\
}

class LuaUtils{

public:
    template<typename T, typename... Args>
    static T *push_new(lua_State *L, Args &&... args) {
        //lua_pushstring(L, "test userdatauv");
        T *addr = (T *) lua_newuserdatauv(L, sizeof(T), 1);
        new(addr) T(std::forward<Args>(args)...);
        luaL_getmetatable(L, get_mtname<T>());
        lua_setmetatable(L, -2);
        return addr;
    }

    template<typename T>
    static void push_obj(lua_State *L, const T &obj) {
        new(lua_newuserdata(L, sizeof(T))) T(obj);
        luaL_getmetatable(L, get_mtname<T>());
        lua_setmetatable(L, -2);
    }

    template<typename T>
    static T *push_ptr(lua_State *L, T *ptr) {
        T **ls = (T **) lua_newuserdata(L, sizeof(T *));
        *ls = ptr;
        //LOGD("push_ptr:  ls = %p, ptr = %p", ls, ptr);
        luaL_getmetatable(L, get_mtname<T>());
        lua_setmetatable(L, -2);
        return ptr;
    }

    template<typename T>
    static T *push_ref(lua_State *L, T *ref) {
        *(T **) lua_newuserdata(L, sizeof(T *)) = SkSafeRef(ref);
        luaL_getmetatable(L, get_mtname<T>());
        lua_setmetatable(L, -2);
        return ref;
    }

    template<typename T>
    static void push_ref(lua_State *L, sk_sp<T> sp) {
        *(T **) lua_newuserdata(L, sizeof(T *)) = sp.release();
        luaL_getmetatable(L, get_mtname<T>());
        lua_setmetatable(L, -2);
    }
    /**
     * cast the index of data to target type. if can't return throw lua error.
     * @tparam T the expect type of data
     * @param L the lua stack
     * @param index the index if ud
     * @return the pointer of data
     */
    template<typename T>
    static T *get_ref(lua_State *L, int index) {
        return *(T **) luaL_checkudata(L, index, get_mtname<T>());
    }

    /**
     * cast the index of data to target type. if can't return null not throw error.
     * @tparam T the expect type of data
     * @param L the lua stack
     * @param index the index if ud
     * @return the pointer of data
     */
    template<typename T>
    static T *to_ref(lua_State *L, int index) {
        void *d = luaL_testudata(L, index, get_mtname<T>());
        return d != nullptr ? *((T **) d) : nullptr;
    }

    template<typename T>
    static T *get_obj(lua_State *L, int index) {
        return (T *) luaL_checkudata(L, index, get_mtname<T>());
    }

    static void setfield_function(lua_State *L,
                                  const char key[], lua_CFunction value) {
        lua_pushcfunction(L, value);
        lua_setfield(L, -2, key);
    }

    static SkScalar lua2scalar(lua_State *L, int index) {
        SkASSERT(lua_isnumber(L, index));
        return SkLuaToScalar(lua_tonumber(L, index));
    }

    static void setarray_number(lua_State *L, int index, double value) {
        lua_pushnumber(L, value);
        lua_rawseti(L, -2, index);
    }

    static void setarray_scalar(lua_State *L, int index, SkScalar value) {
        setarray_number(L, index, SkScalarToLua(value));
    }

    //wrap method for lua get and set. see 'ud_wrap.lua'
    static int forward_call(lua_State *L) { //a.call(a, k, v)
        const char *key = lua_tostring(L, -2);
        // before is {a, k, v};
        lua_pushvalue(L, 2); //{a, k, v, k}
        lua_gettable(L, 1); // {a, k, v, method}
        lua_pushvalue(L, 1); // {a, k, v, method, a}
        lua_pushvalue(L, 3); // {a, k, v, method, a, v}
        //LOGD("---------  ---------");
        //LOGD("before call... %s", key);
        //luaB_dumpStack(L);
        int result = lua_pcall(L, 2, 1, 0); //two arguments, 1 result,
        //LOGD("after call...");
        //luaB_dumpStack(L);
        //LOGD("---------  ---------");
        if (result == LUA_OK) {
            lua_pushvalue(L, -1);
            return 1;
        }
        return 0;
    }

// {a, k, ...} => a.k(a, ...)
    static int forward_func_call(lua_State *L) {
        //a.call  (a, tostring(s), ...):  {a, k, ...} .
        //need: {a, func, a, ...}
        auto n = lua_gettop(L);
        auto key = lua_tostring(L, 2);
        lua_pushvalue(L, 2); //{a, k, ..., k}
        lua_remove(L, 2);   // {a, ..., k}
        lua_gettable(L, 1); // {a, ..., func}
        lua_insert(L, 2);   // {a, func, ...}
        lua_pushvalue(L, 1); // {a, func, ... , a}
        lua_insert(L, 3); // {a, func, a, ...}

        //LOGD("------- before func_call... %s", key);
        //luaB_dumpStack(L);
        int result = lua_pcall(L, n - 1, 1, 0);
        //LOGD("------- after func_call... %d", result); // 0 is ok
        //luaB_dumpStack(L);
        if (result == LUA_OK) {
            lua_pushvalue(L, -1);
            return 1;
        }
        return 0;
    }
    static inline int ref(lua_State *L){
        if(lua_gettop(L) == 0){
            return LUA_NOREF;
        } else if(lua_type(L, -1) == LUA_TNIL){
            return LUA_NOREF;
        } else{
            return luaL_ref(L,LUA_REGISTRYINDEX);
        }
    }
    static inline int ref_get(lua_State *L, int ref){
        return lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    }
    static inline void unref(lua_State *L, int ref){
        if(ref != LUA_NOREF){
            luaL_unref(L, LUA_REGISTRYINDEX, ref);
        }
    }
};

#endif //BGFX_STUDY_LUAUTILS_H
