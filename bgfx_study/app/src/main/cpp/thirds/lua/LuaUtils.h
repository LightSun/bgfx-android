//
// Created by Administrator on 2020/8/28 0028.
//

#ifndef BGFX_STUDY_LUAUTILS_H
#define BGFX_STUDY_LUAUTILS_H

#include "SkRefCnt.h"
#include "lua.hpp"

#define lua2bool(L, idx) lua_toboolean(L, idx) == 1

#define TO_NUMBER_8(L, idx) static_cast<uint8_t>(lua_tonumber(L, idx))
#define TO_NUMBER_16(L, idx) static_cast<uint16_t>(lua_tonumber(L, idx))
#define TO_NUMBER_32(L, idx) static_cast<uint32_t>(lua_tonumber(L, idx))
#define TO_FLOAT(L, idx) static_cast<float>(lua_tonumber(L, idx))

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

class LuaUtils{

public:
    template<typename T, typename... Args>
    static T *push_new(lua_State *L, Args &&... args) {
        T *addr = (T *) lua_newuserdata(L, sizeof(T));
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

    template<typename T>
    static T *get_ref(lua_State *L, int index) {
        return *(T **) luaL_checkudata(L, index, get_mtname<T>());
    }

    template<typename T>
    static T *to_ref(lua_State *L, int index) {
        void *d = luaL_testudata(L, index, get_mtname<T>());
        return d != nullptr ? *((T **) d) : nullptr;
    }

    template<typename T>
    static T *get_obj(lua_State *L, int index) {
        return (T *) luaL_checkudata(L, index, get_mtname<T>());
    }

};

#endif //BGFX_STUDY_LUAUTILS_H
