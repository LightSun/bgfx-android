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
class SkMemoryArray;

class SkCalculator{
public:
    class Data{
    public:
        void * data;
        int length;
        char type;
    };
    class Callback{
    public:
        virtual Data* _mul(SkMemory* m1, SkMemory* m2) = 0;
        virtual Data* _mul(SkAnyMemory* m1, SkAnyMemory* m2) = 0;
        virtual Data* _mul(SkMemory* m1, SkAnyMemory* m2) = 0;
        virtual Data* _mul(SkAnyMemory* m1, SkMemory* m2) = 0;
    };
    int mul(SkMemoryArray* arr, SkMemory* m2, Callback* cb);
};

#endif //BGFX_STUDY_SKCALCULATOR_H
