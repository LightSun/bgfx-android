//
// Created by heaven7 on 2020/8/27 0027.
//

#ifndef BGFX_STUDY_SKMEMORY_H
#define BGFX_STUDY_SKMEMORY_H

#include <stdint.h>
#include <atomic>
#include "lua.hpp"

class AbsSkMemory{

public:
    ~AbsSkMemory();
    AbsSkMemory();
    int ref();
    int unRef();
    void asConstant();
    void destroyData();

public:
    void *data;
    size_t size;
    uint8_t _constant; // 1 is constant. 0 not.
    std::atomic_int _ref;
};

class SkMemory : public AbsSkMemory{

public:

    /**
     * one memory contains can multi table.
     * @param L
     * @param tableCount
     * @param minUnitSize
     */
    SkMemory(lua_State* L, int tableCount, const char* type);
    /**
     * create memory with default data.
     * @param type the data type
     * @param len the array length
     */
    SkMemory(const char* type, int len);

    //------ index start from 0 --------
    uint32_t readUInt32(size_t index);
    uint8_t readUByte(size_t index);
    uint16_t readUShort(size_t index);
    float readFloat(size_t index);

    void writeUInt32(size_t index, uint32_t val);
    void writeUByte(size_t index, uint8_t val);
    void writeUShort(size_t index, uint16_t val);
    void writeFloat(size_t index, float val);

    bool isFloat();
public:
    const char * _dType;

private:
    static int getTotalBytes(lua_State *L, int tableCount, const char *t);
    static int getUnitSize(const char* type);
};

//float,float,float, uint32_t
class SkMemoryFFFUI : public AbsSkMemory{
public:
    SkMemoryFFFUI(lua_State* L, int tableCount);
};

#endif //BGFX_STUDY_SKMEMORY_H
