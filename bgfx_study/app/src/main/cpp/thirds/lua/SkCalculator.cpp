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
    if (src->isSingleType()) {
        SkMemoryMatrix *mat = new SkMemoryMatrix(src->getTypes(), src->getRowCount(),
                                                 src->getColumnCount());
        SkMemory* pMemory;
        for (int i = 0; i < src->getRowCount(); ++i) {
            pMemory = src->array[i];
          //TODO  mat->array[i] = pMemory->_mul(val);
        }
        LuaUtils::push_ptr(L, mat);
        return 1;
    } else{
        const auto mat = new SkMemoryMatrix(src->getRowCount(), false);
        SkAnyMemory * pMemory;
        for (int i = 0; i < src->getRowCount(); ++i) {
            pMemory = src->anyArray[i];
           // mat->anyArray[i] = pMemory->_mul(val);
        }
    }
    return 0;
}

int SkCalculator::mul(lua_State *L, SkMemoryMatrix *src, SkAnyMemory *m2) {
    return 0;
}

int SkCalculator::mul(lua_State *L, SkMemoryMatrix *src, SkMemoryMatrix *m2) {
    return 0;
}