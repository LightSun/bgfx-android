//
// Created by Administrator on 2020/9/15 0015.
//

#ifndef BGFX_STUDY_SKMEMORYARRAY_H
#define BGFX_STUDY_SKMEMORYARRAY_H

#include "SkMemory.h"
#include "lua.hpp"

class SkMemoryArray;

/**
 * any memory object can convert to an array
 */
class SkMemoryArray: public IMemory {

public:
    //element types
    SkMemoryArray(char type, int count);

    ~SkMemoryArray();

    void destroyData();

    bool isValid();

    int getLength(){ return count;}
    char getType(){ return type;}

    void toString(SB::StringBuilder &sb);
    //--------------------------------------------

    SkMemoryArray* copy();
    SkMemoryArray* _mul(double val);

    void* operator[](int index);

    //arr[i] = xxx
    void assignElement(int index, void* val, bool copy);
    bool assignElement(int index, SkMemoryArray* src, bool copy);

    /**
     * operate element with target index and val by multiple
     * @param index the index of element
     * @param val the val to multiple
     * @return the out memory array . may be null , if failed
     */
    bool opElement__mul(int index, SkMemoryArray* out ,double val);
public:
    char type;
    int count;
    SkMemoryArray **arrArray;
    SkMemoryMatrix **matArray;
};


#endif //BGFX_STUDY_SKMEMORYARRAY_H
