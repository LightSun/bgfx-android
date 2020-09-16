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

class SkMemoryHolder{
public:
    char type;
    void* ptr;

    SkMemoryHolder(char type, void *ptr);
};

#define DESTROY_POINTER_ARRAY(array) \
if(array){ \
for (int i = 0; i < count; ++i) { \
auto ptr = array[i]; \
if(ptr != NULL){ \
    ptr->unRefAndDestroy(); \
} \
} \
delete[](array); \
array = nullptr; \
}

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

    /**
     * init array element
     * @param t the data type
     * @param data the total data
     * @param totalIndex the expect index as bytes to init
     */
    static void init(char t, void* data, int totalIndex);

    /**
     * cast data from src to dst
     * @param srcData  the src data
     * @param srcType  the src data type
     * @param dstData
     * @param dstType
     * @param index
     */
    static void convert(void *srcData, const char srcType, void *dstData, const char dstType, int index);

    static void convert(void *srcData, char srcType, size_t srcBytes,
            void *dstData, const char dstType, size_t dstBytes);

    static double getValue(void *data, const char type, size_t bytesIndex);
    /**
     * multi the single data from one to another.
     * @param srcData the src data
     * @param dstType the type
     * @param totalIndex the total index as bytes
     * @param val the value
     */
    static void multiple(void *srcData, const char type, size_t totalIndex, double val);
    static void multiple(void *srcData, const char type, size_t totalIndex, lua_Integer val);

    static void multiple(void *srcData,const char srcType, size_t srcBytes,
            void* dstData, const char dstType, size_t dstBytes,
            void *outData, const char outType, size_t outBytes);

    static bool isSigned(const char type);

    /** compute a correct type */
    static const char computeType(const char type, const char type1);
    //result must be a signed type.
    /**
     * upgrade unsigned type to a signed type
     * @param type the unsigned type
     * @param hasFloat has float or not
     * @return the upgrade type
     */
    static const char upgradeType(const char type, const bool hasFloat);
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

    void unRefAndDestroy();

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

    /**
     * convert memory type to target type. note the length must match.
     * put a new memory of target type. may be SkMemory/SkAnyMemory.
     * @param L: the lua stack
     * @param ts: the types of memory. like 'f', 'fff', 'df'...etc.
     * @return a new memory of target type. may be SkMemory/SkAnyMemory
     */
    int convert(lua_State* L, const char* ts){ return 0; }

    int foreach(lua_State* L){ return 0;}

public:
    void *data;
    size_t size;
};

#endif //BGFX_STUDY_IMEMORY_H
