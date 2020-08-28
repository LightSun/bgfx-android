//
// Created by Administrator on 2020/8/28 0028.
//

#ifndef BGFX_STUDY_INT64_H
#define BGFX_STUDY_INT64_H

#include "log.h"
#include "lua.hpp"
#include "global.h"

extern "C" {

#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
}

template<typename T, typename Ptr>
class BigInteger;

#define BigInt64(m) BigInteger<int64_t,intptr_t>::m
#define BigUint64(m) BigInteger<uint64_t,uintptr_t>::m

//copy and changed from 'https://github.com/cloudwu/lua-int64/blob/master/int64.c'
template<typename T, typename Ptr>
class BigInteger {
public:
    //table param: 1(lua_Number/LUA_TSTRING/LUA_TLIGHTUSERDATA)
    static T
    _get(lua_State *L, int index) {
        int type = lua_type(L, index);
        T n = 0;
        switch (type) {
            case LUA_TNUMBER: {
                lua_Number d = lua_tonumber(L, index);
                n = (T) d;
                break;
            }
            case LUA_TSTRING: {
                size_t len = 0;
                const uint8_t *str = (const uint8_t *) lua_tolstring(L, index, &len);
                if (len > 8) {
                    return luaL_error(L, "The string (length = %d) is not an int64 string", len);
                }
                int i = 0;
                T n64 = 0;
                for (i = 0; i < (int) len; i++) {
                    n64 |= (T) str[i] << (i * 8);
                }
                n = (T) n64;
                break;
            }
            case LUA_TLIGHTUSERDATA: {
                void *p = lua_touserdata(L, index);
                n = (Ptr) p;
                break;
            }
            default:
                return luaL_error(L, "argument %d error type %s", index, lua_typename(L, type));
        }
        return n;
    }

    static inline void
    _set(lua_State *L, T n) {
        void *p = (void *) (Ptr) n;
        lua_pushlightuserdata(L, p);
    }

    static int _add(lua_State *L) {
        T a = _get(L, 1);
        T b = _get(L, 2);
        _set(L, a + b);
        return 1;
    }

    static int
    _sub(lua_State *L) {
        T a = _get(L, 1);
        T b = _get(L, 2);
        _set(L, a - b);
        return 1;
    }

    static int
    _mul(lua_State *L) {
        T a = _get(L, 1);
        T b = _get(L, 2);
        _set(L, a * b);
        return 1;
    }

    static int
    _div(lua_State *L) {
        T a = _get(L, 1);
        T b = _get(L, 2);
        if (b == 0) {
            return luaL_error(L, "div by zero");
        }
        _set(L, a / b);

        return 1;
    }

    static int
    _mod(lua_State *L) {
        T a = _get(L, 1);
        T b = _get(L, 2);
        if (b == 0) {
            return luaL_error(L, "mod by zero");
        }
        _set(L, a % b);
        return 1;
    }

    static T
    _pow64(T a, T b) {
        if (b == 1) {
            return a;
        }
        T a2 = a * a;
        if (b % 2 == 1) {
            return _pow64(a2, b / 2) * a;
        } else {
            return _pow64(a2, b / 2);
        }
    }

    static int
    _pow(lua_State *L) {
        T a = _get(L, 1);
        T b = _get(L, 2);
        T p;
        if (b > 0) {
            p = _pow64(a, b);
        } else if (b == 0) {
            p = 1;
        } else {
            return luaL_error(L, "pow by nagtive number %d", (int) b);
        }
        _set(L, p);
        return 1;
    }

    //negative
    static int
    _unm(lua_State *L) {
        T a = _get(L, 1);
        _set(L, -a);
        return 1;
    }

    static int
    _new(lua_State *L) {
        int top = lua_gettop(L);
        T n;
        switch (top) {
            case 0 :
                lua_pushlightuserdata(L, NULL);
                break;
            case 1 :
                n = _get(L, 1);
                _set(L, n);
                break;
            default: {
                //base 代表进制
                int base = luaL_checkinteger(L, 2);
                if (base < 2) {
                    luaL_error(L, "base must be >= 2");
                }
                const char *str = luaL_checkstring(L, 1);
                n = strtoll(str, NULL, base);
                _set(L, n);
                break;
            }
        }
        return 1;
    }

    static int
    _eq(lua_State *L) {
        T a = _get(L, 1);
        T b = _get(L, 2);
        LOGD("int64_eq : %s %s\n", lua_typename(L, 1), lua_typename(L, 2));
        LOGD("int64_eq :  %ld %ld\n", a, b);
        lua_pushboolean(L, a == b);
        return 1;
    }

    static int
    _lt(lua_State *L) {
        T a = _get(L, 1);
        T b = _get(L, 2);
        lua_pushboolean(L, a < b);
        return 1;
    }

    static int
    _le(lua_State *L) {
        T a = _get(L, 1);
        T b = _get(L, 2);
        lua_pushboolean(L, a <= b);
        return 1;
    }

    static int
    _len(lua_State *L) {
        T a = _get(L, 1);
        lua_pushnumber(L, (lua_Number) a);
        return 1;
    }

    static int
    _tostring(lua_State *L) {
        static char hex[17] = "0123456789ABCDEF"; // last is \0
        uintptr_t n = (uintptr_t) lua_touserdata(L, 1);
        if (lua_gettop(L) == 1) {
            luaL_Buffer b;
            luaL_buffinitsize(L, &b, 28);
            luaL_addstring(&b, "int64: 0x");
            int i;
            bool strip = true;
            for (i = 15; i >= 0; i--) {
                int c = (n >> (i * 4)) & 0xf;
                if (strip && c == 0) {
                    continue;
                }
                strip = false;
                luaL_addchar(&b, hex[c]);
            }
            if (strip) {
                luaL_addchar(&b, '0');
            }
            luaL_pushresult(&b);
        } else {
            int base = luaL_checkinteger(L, 2);
            int shift, mask;
            switch (base) {
                case 0: {
                    unsigned char buffer[8];
                    int i;
                    for (i = 0; i < 8; i++) {
                        buffer[i] = (n >> (i * 8)) & 0xff;
                    }
                    lua_pushlstring(L, (const char *) buffer, 8);
                    return 1;
                }
                case 10: {
                    T dec = (T) n;
                    luaL_Buffer b;
                    luaL_buffinitsize(L, &b, 28);
                    if (dec < 0) {
                        luaL_addchar(&b, '-');
                        dec = -dec;
                    }
                    int buffer[32];
                    int i;
                    for (i = 0; i < 32; i++) {
                        buffer[i] = dec % 10;
                        dec /= 10;
                        if (dec == 0)
                            break;
                    }
                    while (i >= 0) {
                        luaL_addchar(&b, hex[buffer[i]]);
                        --i;
                    }
                    luaL_pushresult(&b);
                    return 1;
                }
                case 2:
                    shift = 1;
                    mask = 1;
                    break;
                case 8:
                    shift = 3;
                    mask = 7;
                    break;
                case 16:
                    shift = 4;
                    mask = 0xf;
                    break;
                default:
                    luaL_error(L, "Unsupport base %d", base);
                    break;
            }
            int i;
            char buffer[64];
            for (i = 0; i < 64; i += shift) {
                buffer[i / shift] = hex[(n >> (64 - shift - i)) & mask];
            }
            lua_pushlstring(L, buffer, 64 / shift);
        }
        return 1;
    }
};

extern "C" int luaopen_int64(lua_State *L);
extern "C" int luaopen_uint64(lua_State *L);

#endif //BGFX_STUDY_INT64_H
