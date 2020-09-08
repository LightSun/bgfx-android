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
class MemoryUtils;
class MemoryAllocator;

//push string and then release
#define MEM_PUSH_TO_STRING(L, ptr) \
const char* str = ((IMemory*)ptr)->toString(); \
lua_pushstring(L, str);\
free((void*)str);

class MemoryUtils{
public:
    static int getUnitSize(char type);
    /**
     * read data from lua stack
     * @param L the lua stack
     * @param type the type
     * @param data the data
     * @param totalIndex total index as bytes
     * @return the result
     */
    static int read(lua_State *L, char t, void* data, int totalIndex);
    /**
     * write single data to lua stack
     * @param L the lua stack
     * @param t the type of single data
     * @param data the data addr
     * @param totalIndex the total bytes index
     * @return the result
     */
    static int write(lua_State* L, char t, void* data, int totalIndex);

    /**
    * read data as string to string builder
    * @param sb the string builder
    * @param type the type
    * @param data the data
    * @param totalIndex total index as bytes
    */
    static void toString(SB::StringBuilder& sb, char t, void* data, int totalIndex);
};

class MemoryAllocator{

public:
    MemoryAllocator();
    ~MemoryAllocator();

    void* require(char type);
    void releaseAll();

private:
    float* f;
    uint8_t * u8;
    uint16_t * u16;
    uint32_t * u32;
};

class IMemory{

public:

    IMemory();

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
