//
// Created by Administrator on 2020/9/14 0014.
//

#include <math.h>
#include "SkCalculator.h"
#include "SkMemory.h"
#include "LuaUtils.h"

//TODO wait SkCalculator
int SkCalculator::mul(lua_State *L, SkMemoryMatrix *src, SkMemory *m2) {
    return 0;
}

int SkCalculator::mul(lua_State *L, SkMemoryMatrix *src, double val) {
    const auto mat = src->_mul(val);
    LuaUtils::push_ptr(L, mat);
    return 1;
}

int SkCalculator::mul(lua_State *L, SkMemoryMatrix *src, SkAnyMemory *m2) {
    return 0;
}

int SkCalculator::mul(lua_State *L, SkMemoryMatrix *src, SkMemoryMatrix *m2) {
    return 0;
}