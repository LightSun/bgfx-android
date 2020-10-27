//
// Created by Administrator on 2020/10/27 0027.
//

#ifndef BGFX_STUDY_SKMEMORYUTILS_H
#define BGFX_STUDY_SKMEMORYUTILS_H

#include "SkHolder.h"

class SkMemoryMatrix;
class SkMemory;
class SkAnyMemory;

class SkMemoryUtils {
public:
    //int rowCount, int colCount, char type, double defVal
    //return error msg
    static void reshapeMat(SkMemoryMatrix* mat, const char* mode, int rowC, int colC,
            char t, double defVal, SkHolder* out);

    static void reshapeMem(SkMemory* mem, int rowC, int colC,
                                  char t, double defVal, SkHolder* out);

    static void reshapeMem(SkAnyMemory* mem, int rowC, int colC,
                                  char t, double defVal, SkHolder* out);
};

#endif //BGFX_STUDY_SKMEMORYUTILS_H
