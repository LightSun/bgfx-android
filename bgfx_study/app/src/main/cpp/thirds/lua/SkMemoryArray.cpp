//
// Created by Administrator on 2020/9/15 0015.
//

#include "SkMemoryArray.h"
#include "stringbuilder.h"
#include "global.h"
#include "log.h"


SkMemoryArray::SkMemoryArray() {
    array = nullptr;
    anyArray = nullptr;
    matArray = nullptr;
    arrArray = nullptr;
    type = TYPE_NONE;
}

SkMemoryArray::SkMemoryArray(char type, int count): type(type), count(count) {
    switch (type){
        case TYPE_MEM:
            array = new SkMemory*[count];
            break;
        case TYPE_MEM_ANY:
            anyArray = new SkAnyMemory*[count];
            break;
        case TYPE_MEM_ARRAY:
            arrArray = new SkMemoryArray*[count];
            break;
        case TYPE_MEM_MAT:
            matArray = new SkMemoryMatrix*[count];
            break;
        default:
            LOGE("wrong type = %d", type);
    }
}

SkMemoryArray::~SkMemoryArray() {
    destroyData();
}

void SkMemoryArray::destroyData() {
    switch (type){
        case TYPE_MEM:
            DESTROY_POINTER_ARRAY(array)
            break;
        case TYPE_MEM_ANY:
            DESTROY_POINTER_ARRAY(anyArray)
            break;
        case TYPE_MEM_ARRAY:
            DESTROY_POINTER_ARRAY(arrArray)
            break;
        case TYPE_MEM_MAT:
            DESTROY_POINTER_ARRAY(matArray)
            break;
    }
    type = TYPE_NONE;
}

bool SkMemoryArray::isValid() {
    return type > 0;
}

#define ARR_TOSTRING(arr) \
for( int i = 0; i < count ; i ++){ \
    arr[i]->toString(sb); \
    if(i != count - 1){ \
        sb << ", \n"; \
    } \
}
void SkMemoryArray::toString(SB::StringBuilder &sb) {
    sb << "[";
    switch (type){
        case TYPE_MEM:
            ARR_TOSTRING(array)
            break;
        case TYPE_MEM_ANY:
            ARR_TOSTRING(anyArray)
            break;
        case TYPE_MEM_ARRAY:
            ARR_TOSTRING(arrArray)
            break;
        case TYPE_MEM_MAT:
            ARR_TOSTRING(matArray)
            break;
    }
    sb << "]";
}