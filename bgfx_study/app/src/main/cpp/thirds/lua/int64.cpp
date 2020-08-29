//
// Created by Administrator on 2020/8/28 0028.
//
#include "int64.h"

static void make_meta_int64(lua_State *L) {
    luaL_Reg lib[] = {
            { "__add", BigInt64(_add) },
            { "__sub", BigInt64(_sub) },
            { "__mul", BigInt64(_mul) },
            { "__div", BigInt64(_div) },
            { "__mod", BigInt64(_mod) },
            { "__unm", BigInt64(_unm) },
            { "__pow", BigInt64(_pow) },
            { "__bor", BigInt64(_bor) },
            { "__eq", BigInt64(_eq) },
            { "__lt", BigInt64(_lt) },
            { "__le", BigInt64(_le) },
            { "__len", BigInt64(_len) },
            { "__tostring", BigInt64(_tostring)},
            { NULL, NULL },
    };
    luaL_newlib(L,lib);
}

static void make_meta_uint64(lua_State *L) {
    luaL_Reg lib[] = {
            { "__add", BigUint64(_add) },
            { "__sub", BigUint64(_sub) },
            { "__mul", BigUint64(_mul) },
            { "__div", BigUint64(_div) },
            { "__mod", BigUint64(_mod) },
            { "__unm", BigUint64(_unm) },
            { "__pow", BigUint64(_pow) },
            { "__bor", BigUint64(_bor) },
            { "__eq", BigUint64(_eq) },
            { "__lt", BigUint64(_lt) },
            { "__le", BigUint64(_le) },
            { "__len", BigUint64(_len) },
            { "__tostring", BigUint64(_tostring)},
            { NULL, NULL },
    };
    luaL_newlib(L, lib);
}

extern "C" int luaopen_int64(lua_State *L) {
    if (sizeof(intptr_t) != sizeof(int64_t)) {
        return luaL_error(L, "Only support 64bit architecture");
    }
    lua_pushlightuserdata(L, NULL);
    make_meta_int64(L);
    lua_setmetatable(L,-2);
    lua_pop(L,1);

    lua_newtable(L);
    lua_pushcfunction(L, BigInt64(_new));
    lua_setfield(L, -2, "new");
    lua_pushcfunction(L, BigInt64(_tostring));
    lua_setfield(L, -2, "tostring");

    return 1;
}
//intptr_t and uintptr_t:   https://blog.csdn.net/cs_zhanyb/article/details/16973379
extern "C" int luaopen_uint64(lua_State *L) {
    if (sizeof(uintptr_t) != sizeof(uint64_t)) {
        return luaL_error(L, "Only support u-64bit architecture");
    }
    lua_pushlightuserdata(L, NULL);
    make_meta_uint64(L);
    lua_setmetatable(L,-2);
    lua_pop(L,1);

    lua_newtable(L);
    lua_pushcfunction(L, BigUint64(_new));
    lua_setfield(L, -2, "new");
    lua_pushcfunction(L, BigUint64(_tostring));
    lua_setfield(L, -2, "tostring");

    return 1;
}