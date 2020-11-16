//
// Created by Administrator on 2020/9/11 0011.
//

#include <bx/math.h>
#include <bx/timer.h>
#include <bx/file.h>
#include <bgfx_utils.h>
#include <common.h>

#include "bx_lua.h"
#include "LuaUtils.h"
#include "SkMemory.h"
#include "int64.h"
#include "bgfx_wrapper.h"

//--------------------------------------------------------
static int bx_getHPCounter(lua_State *L) {
    auto i = bx::getHPCounter();
    BigInt64(_set(L, i));
    return 1;
}

static int bx_getHPFrequency(lua_State *L) {
    auto i = bx::getHPFrequency();
    BigInt64(_set(L, i));
    return 1;
}

//bx::Vec3
static int bx_newVec3(lua_State *L) {
    bx::Vec3 *pvec3 = new bx::Vec3(TO_FLOAT(L, 1), TO_FLOAT(L, 2), TO_FLOAT(L, 3));
    LuaUtils::push_ptr(L, pvec3);
    return 1;
}

static int bx_mtxLookAt(lua_State *L) {
    //bx::mtxLookAt(view, eye, at); (array, Vec3, Vec3)
    auto *pMemory = LuaUtils::get_ref<SkMemory>(L, 1);
    auto *eye = LuaUtils::get_ref<bx::Vec3>(L, 2);
    auto *at = LuaUtils::get_ref<bx::Vec3>(L, 3);
    auto *_up = LuaUtils::to_ref<bx::Vec3>(L, 4);
    const char *type = lua_tostring(L, 5);
    bx::Handness::Enum en =
            type != nullptr ? bgfx_handness_enum(L, type) : bx::Handness::Enum::Left;
    if (!pMemory->isFloat()) {
        return luaL_error(L, "for bx::mtxLookAt. memory data type must be float.");
    }
    if (_up == nullptr) {
        bx::Vec3 up = bx::Vec3(0.0f, 1.0f, 0.0f);
        _up = &up;
    }
    //_up and en had default parameter
    bx::mtxLookAt(static_cast<float *>(pMemory->data), *eye, *at, *_up, en);
    return 0;
}

//normal 6
static inline int bx_mtxProj6(lua_State *L) {
    auto *pMemory = LuaUtils::get_ref<SkMemory>(L, 1);
    auto *_fovy = LuaUtils::get_ref<SkMemory>(L, 2);
    auto _near = TO_FLOAT(L, 3);
    auto _far = TO_FLOAT(L, 4);
    bool homogeneousNdc = lua2bool(L, 5);
    const char *handness = lua_tostring(L, 6);
    bx::Handness::Enum en =
            handness != nullptr ? bgfx_handness_enum(L, handness) : bx::Handness::Enum::Left;
    if (!pMemory->isFloat()) {
        return luaL_error(L, "for bx::mtxProj. memory data type must be float.");
    }
    if (!_fovy->isFloat()) {
        return luaL_error(L, "for bx::mtxProj. _fovy memory data type must be float.");
    }
    //float[4]
    if (_fovy->size != 4 * 4) {
        return luaL_error(L, "for bx::mtxProj. _fovy memory data must be float[4].");
    }
    bx::mtxProj(static_cast<float *>(pMemory->data), static_cast<float *>(_fovy->data), _near, _far,
                homogeneousNdc, en);
    return 0;
}

static inline int bx_mtxProj7(lua_State *L) {
    //  float* _result, float _fovy, float _aspect, float _near, float _far, bool _homogeneousNdc, Handness::Enum _handness = Handness::Left
    auto *pMemory = LuaUtils::get_ref<SkMemory>(L, 1);
    auto _fovy = TO_FLOAT(L, 2);
    auto _aspect = TO_FLOAT(L, 3);
    auto _near = TO_FLOAT(L, 4);
    auto _far = TO_FLOAT(L, 5);
    bool homogeneousNdc = lua2bool(L, 6);
    const char *handness = lua_tostring(L, 7);
    bx::Handness::Enum en =
            handness != nullptr ? bgfx_handness_enum(L, handness) : bx::Handness::Enum::Left;
    if (!pMemory->isFloat()) {
        return luaL_error(L, "for bx::mtxProj. memory data type must be float.");
    }
    bx::mtxProj(static_cast<float *>(pMemory->data), _fovy, _aspect, _near, _far,
                homogeneousNdc, en);
    return 0;
}

static int bx_mtxProj(lua_State *L) {
    auto n = lua_gettop(L);
    switch (n) {
        case 8:
        case 9: {
            SkMemory *pMemory = LuaUtils::get_ref<SkMemory>(L, 1);
            auto _ut = TO_FLOAT(L, 2);
            auto _dt = TO_FLOAT(L, 3);
            auto _lt = TO_FLOAT(L, 4);
            auto _rt = TO_FLOAT(L, 5);
            auto _near = TO_FLOAT(L, 6);
            auto _far = TO_FLOAT(L, 7);
            bool homogeneousNdc = lua2bool(L, 8);
            const char *handness = lua_tostring(L, 9);
            bx::Handness::Enum en =
                    handness != nullptr ? bgfx_handness_enum(L, handness)
                                        : bx::Handness::Enum::Left;
            if (!pMemory->isFloat()) {
                return luaL_error(L, "for bx::mtxProj. memory data type must be float.");
            }
            bx::mtxProj(static_cast<float *>(pMemory->data), _ut, _dt, _lt, _rt, _near, _far,
                        homogeneousNdc, en);
            return 0;
        }
        case 7: {
            return bx_mtxProj7(L);
        }
        case 5:
            return bx_mtxProj6(L);

        case 6: {
            //can be 6 or 7. for Handness is opt parameter.
            if (lua_type(L, 6) == LUA_TBOOLEAN) {
                return bx_mtxProj7(L);
            }
            return bx_mtxProj6(L);
        }
        default:
            return luaL_error(L, "wrong arguments for bx::mtxProj.");
    }
    return 0;
}

static int bx_mtxRotateXY(lua_State *L) {
    //bx::mtxRotateXY(mtx, time + xx*0.21f, time + yy*0.37f);
    SkMemory *pMemory = LuaUtils::get_ref<SkMemory>(L, 1);
    float ax = TO_FLOAT(L, 2);
    float ay = TO_FLOAT(L, 3);
    if (!pMemory->isFloat()) {
        return luaL_error(L, "for bx::mtxRotateXY. memory data type must be float.");
    }
    bx::mtxRotateXY(static_cast<float *>(pMemory->data), ax, ay);
    return 0;
}
static int utils_loadMem(lua_State *L) {
    bx::FileReader reader;
    auto pMemory = loadMem(&reader, lua_tostring(L, 1));
    LuaUtils::push_ptr(L, pMemory);
    return 1;
}
static int utils_loadMemFromAssets(lua_State *L) {
    auto pMemory = loadMem(entry::getFileReader(), lua_tostring(L, 1));
    LuaUtils::push_ptr(L, pMemory);
    return 1;
}

//-------------------- bx::Vec3 --------------------
static int vec3_gc(lua_State *L) {
    bx::Vec3 *pVec3 = LuaUtils::get_ref<bx::Vec3>(L, -1);
    delete pVec3;
    return 0;
}

namespace gbx {
    static const luaL_Reg Vec3_Methods[] = {
            {"__gc", vec3_gc},
            {NULL, NULL},
    };
}
static const luaL_Reg bx_funcs[] = {
        {"getHPCounter",   bx_getHPCounter},
        {"getHPFrequency", bx_getHPFrequency},
        {"newVec3",        bx_newVec3},
        {"mtxLookAt",      bx_mtxLookAt},
        {"mtxProj",        bx_mtxProj},
        {"mtxRotateXY",    bx_mtxRotateXY},
        //help methods
        {"loadMem",            utils_loadMem},
        {"loadMemFromAssets",  utils_loadMemFromAssets},
        {nullptr,          nullptr}
};
extern "C" int luaopen_bx_lua(lua_State *L) {
    luaL_newlib(L, bx_funcs);
    return 1;
}

DEF_MTNAME(bx::Vec3)
void BxLua::registers(lua_State *L) {
    REG_CLASS(L, bx::Vec3);
}