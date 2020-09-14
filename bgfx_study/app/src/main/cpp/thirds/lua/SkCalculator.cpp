//
// Created by Administrator on 2020/9/14 0014.
//

#include "SkCalculator.h"
#include "SkMemory.h"

//TODO wait SkCalculator
int SkCalculator::mul(lua_State* L, SkMemoryMatrix *src, SkMemory *m2) {
    return 0;
}
int SkCalculator::mul(lua_State* L, SkMemoryMatrix *src, double val) {
     if(src->isSingleType()){
         SkMemoryMatrix * mat = new SkMemoryMatrix(src->getTypes(), src->getRowCount(), src->getColumnCount());
         char srcType;
         int unitSize;

         for (int i = 0; i < src->getRowCount(); ++i) {
             auto pMemory = src->array[i];
             srcType = pMemory->_dType[0];
             unitSize = MemoryUtils::getUnitSize(srcType);
             for (int j = 0; j < pMemory->getLength(); ++j) {
                //TODO MemoryUtils::mul(pMemory->data, srcType, j * unitSize, mat[i], val);
             }
         }
     }
    return 0;
}
int SkCalculator::mul(lua_State* L, SkMemoryMatrix *src, SkAnyMemory *m2) {
    return 0;
}
int SkCalculator::mul(lua_State* L, SkMemoryMatrix *src, SkMemoryMatrix *m2) {
    return 0;
}