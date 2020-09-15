//
// Created by Administrator on 2020/9/15 0015.
//

#ifndef BGFX_STUDY_SKMEMORYARRAY_H
#define BGFX_STUDY_SKMEMORYARRAY_H

#include "SkMemory.h"

/**
 * any memory object can convert to an array
 */
class SkMemoryArray: public IMemory {

public:
    //element types
    const static char TYPE_NONE      = 0;
    const static char TYPE_MEM       = 1;
    const static char TYPE_MEM_ANY   = 2;
    const static char TYPE_MEM_ARRAY = 3;
    const static char TYPE_MEM_MAT   = 4;

    SkMemoryArray();
    SkMemoryArray(char type, int count);

    ~SkMemoryArray();

    void destroyData();

    bool isValid();

    int getLength(){ return count;}
    char getType(){ return type;}

    void toString(SB::StringBuilder &sb);

public:
    char type;
    int count;
    SkMemory **array;
    SkAnyMemory **anyArray;
    SkMemoryArray **arrArray;
    SkMemoryMatrix **matArray;
};


#endif //BGFX_STUDY_SKMEMORYARRAY_H
