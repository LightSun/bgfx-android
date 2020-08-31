//
// Created by heaven7 on 2020/8/27 0027.
//

#ifndef BGFX_STUDY_SKMEMORY_H
#define BGFX_STUDY_SKMEMORY_H

#include <stdint.h>
#include <atomic>
#include "lua.hpp"

class SkMemory;
class SkMemoryFFFUI;
class SkMemoryMatrix;

class AbsSkMemory{

public:
    ~AbsSkMemory();
    AbsSkMemory();
    int ref();
    int unRef();
    void asConstant();
    void destroyData();

    int getLength();
    const char* toString() ;
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

    //startIndex: table start index, from 0 means lua stack from 1
    SkMemory(lua_State* L, int startIndex, int tableCount, const char* type);

    //------ index start from 0 --------
    uint32_t readUInt32(size_t index);
    uint8_t readUInt8(size_t index);
    uint16_t readUInt16(size_t index);
    float readFloat(size_t index);

    void writeUInt32(size_t index, uint32_t val);
    void writeUInt8(size_t index, uint8_t val);
    void writeUInt16(size_t index, uint16_t val);
    void writeFloat(size_t index, float val);

    bool isFloat();
    const char* toString() ;

    static int read(SkMemory* mem, lua_State* L);
    static int write(SkMemory* mem, lua_State* L);

    int getLength();

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

    int getLength();
    const char* toString();

    static int read(SkMemoryFFFUI* mem, lua_State* L);
    static int write(SkMemoryFFFUI* mem, lua_State* L);
};
class SkMemoryMatrix{
public:
    ~SkMemoryMatrix();
    /**
     * create memory matrix from multi table values which is from lua_state
     * @param L lua stack
     * @param type
     */
    SkMemoryMatrix(lua_State* L, const char* type);
    /**
     * create empty memory matrix.
     * @param type the type
     * @param rowCount the row count
     * @param columnCount the column count
     */
    SkMemoryMatrix(const char* type, int rowCount, int columnCount);

    void destroyData();

    bool isValid();

    int getRowCount();
    int getColumnCount();
    const char* toString();

    static int read(SkMemoryMatrix* mem, lua_State* L, void (*Push)(lua_State* L, SkMemory* ptr));
    static int write(SkMemoryMatrix* mem, lua_State* L, SkMemory* (*Pull)(lua_State* L, int idx));

private:
    SkMemory** array;
    int count;
};

#endif //BGFX_STUDY_SKMEMORY_H
