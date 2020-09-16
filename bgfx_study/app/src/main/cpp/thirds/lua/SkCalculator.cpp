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
    const auto mat = new SkMemoryMatrix(src->getRowCount(), src->isSingleType());
    if (src->isSingleType()) {
        for (int i = 0; i < src->getRowCount(); ++i) {
            mat->array[i] = src->array[i]->_mul(val);
        }
    } else{
        for (int i = 0; i < src->getRowCount(); ++i) {
            mat->anyArray[i] = src->anyArray[i]->_mul(val);
        }
    }
    LuaUtils::push_ptr(L, mat);
    return 1;
}

int SkCalculator::mul(lua_State *L, SkMemoryMatrix *src, SkAnyMemory *m2) {
    return 0;
}

int SkCalculator::mul(lua_State *L, SkMemoryMatrix *src, SkMemoryMatrix *m2) {
    return 0;
}