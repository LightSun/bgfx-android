//
// Created by Administrator on 2020/9/3 0003.
//

#ifndef BGFX_STUDY_IMEMORY_H
#define BGFX_STUDY_IMEMORY_H

#include <stdint.h>
#include "lua.hpp"
#include <atomic>

namespace SB{
    class StringBuilder;
}
class IMemory;
class SimpleMemory;

//push string and then release
#define MEM_PUSH_TO_STRING(L, ptr) \
const char* str = ((IMemory*)ptr)->toString(); \
lua_pushstring(L, str);\
free((void*)str);

class IMemory{

public:

    IMemory();
    ~IMemory();

    int ref();
    int unRef();

    virtual void destroyData() = 0;
    virtual bool isValid() = 0;

    virtual int getLength() = 0;
    virtual void toString(SB::StringBuilder& sb) = 0;
    /**
     * get the memory string. you need to free the char*, after use.
     * @return the chars
     */
    const char* toString();

public:
    std::atomic_int _ref;
};

/**
 * single type memory. like float, int, uint...etc.
 */
class SimpleMemory :public IMemory{

public:
    SimpleMemory();
    ~SimpleMemory();

    void destroyData();
    bool isValid();

public:
    void *data;
    size_t size;
};

#endif //BGFX_STUDY_IMEMORY_H
