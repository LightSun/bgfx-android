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
        SkMemory* pMemory;
        for (int i = 0; i < src->getRowCount(); ++i) {
            pMemory = src->array[i]->copy();
            pMemory->_mul(val);
            mat->array[i] = pMemory;
        }
    } else{
        SkAnyMemory * pMemory;
        for (int i = 0; i < src->getRowCount(); ++i) {
            pMemory = src->anyArray[i]->copy();
            pMemory->_mul(val);
            mat->anyArray[i] = pMemory;
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