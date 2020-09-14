//
// Created by Administrator on 2020/9/14 0014.
//

#ifndef BGFX_STUDY_SKCALCULATOR_H
#define BGFX_STUDY_SKCALCULATOR_H

#include "lua.hpp"

class IMemory;
class SkMemory;
class SkAnyMemory;
class SkMemoryMatrix;

class SkCalculator{
public:
    static int mul(lua_State*L, SkMemoryMatrix* src, SkMemory* m2);    //SkMemoryMatrix array
    static int mul(lua_State*L, SkMemoryMatrix* src, SkAnyMemory* m2); //SkMemoryMatrix array
    static int mul(lua_State*L, SkMemoryMatrix* src, SkMemoryMatrix* m2); //SkMemoryMatrix
    static int mul(lua_State*L, SkMemoryMatrix* src, double val); //SkMemoryMatrix
};

#endif //BGFX_STUDY_SKCALCULATOR_H
