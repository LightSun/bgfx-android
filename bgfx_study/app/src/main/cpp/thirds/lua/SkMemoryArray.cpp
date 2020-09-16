//
// Created by Administrator on 2020/9/15 0015.
//

#include "SkMemoryArray.h"
#include "stringbuilder.h"
#include "global.h"
#include "log.h"


SkMemoryArray::SkMemoryArray(char type, int count):IMemory(), type(type), count(count) {
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
    count = 0;
}

SkMemoryArray* SkMemoryArray::copy() {
    auto ska = new SkMemoryArray(type, count);
    for (int i = 0; i < count; ++i) {
        ska->assignElement(i, this, true);
    }
    return ska;
}

bool SkMemoryArray::isValid() {
    return type > 0;
}
void SkMemoryArray::toString(SB::StringBuilder &sb) {
#define ARR_TOSTRING(arr) \
for( int i = 0; i < count ; i ++){ \
    arr[i]->toString(sb); \
    if(i != count - 1){ \
        sb << ", \n"; \
    } \
}
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
const SkMemoryHolder& SkMemoryArray::operator[](int index) {
    switch (type){
        case TYPE_MEM:
            return SkMemoryHolder(type, array[index]);
        case TYPE_MEM_ANY:
            return SkMemoryHolder(type, anyArray[index]);
        case TYPE_MEM_ARRAY:
            return SkMemoryHolder(type, arrArray[index]);
        case TYPE_MEM_MAT:
            return SkMemoryHolder(type, matArray[index]);
        default:
            LOGE("wrong type = %d", type);
            return SkMemoryHolder(type, nullptr);
    }
}
void SkMemoryArray::assignElement(int i, void* ptr, bool copy) {
#define ASSIGN(name, type, copy) \
{ \
    auto mem = static_cast<type *>(ptr); \
if (copy) {\
    mem = mem->copy(); \
    } else {\
    mem->ref(); \
    } \
 name[i] = mem; \
}

    switch (type){
        case TYPE_MEM:
            ASSIGN(array, SkMemory, copy)
            break;
        case TYPE_MEM_ANY:
            ASSIGN(anyArray, SkAnyMemory, copy)
            break;
        case TYPE_MEM_ARRAY:
            ASSIGN(arrArray, SkMemoryArray, copy)
            break;
        case TYPE_MEM_MAT:
            ASSIGN(matArray, SkMemoryMatrix, copy)
            break;
        default:
            LOGE("wrong type = %d", type);
    }
}

#define COPY_IF(name, copy) \
if(copy){ \
name[i] = src->name[i]->copy(); \
}else{ \
auto ptr = src->name[i]; \
ptr->ref(); \
name[i] = ptr; \
}

bool SkMemoryArray::assignElement(int i, SkMemoryArray *src, bool copy) {
    if(type != src->type){
        return false;
    }
    switch (type){
        case TYPE_MEM:
            COPY_IF(array, copy)
            break;
        case TYPE_MEM_ANY:
            COPY_IF(anyArray, copy)
            break;
        case TYPE_MEM_ARRAY:
            COPY_IF(arrArray, copy)
            break;
        case TYPE_MEM_MAT:
            COPY_IF(matArray, copy)
            break;
        default:
            LOGE("wrong type = %d", type);
    }
}