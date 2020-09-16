//
// Created by heaven7 on 2020/8/27 0027.
// currently for memory data. support types are: d(uint32), w(uint16), b(uint8),f(float),
//                                               i(int),  s(short), c(char), F(double)
//

#ifndef BGFX_STUDY_SKMEMORY_H
#define BGFX_STUDY_SKMEMORY_H

#include "lua.hpp"
#include "IMemory.h"

class SkMemory;

class SkAnyMemory;

class SkMemoryMatrix;

//python 矩阵运算.
//https://www.cnblogs.com/llxxs/p/11228119.html
//https://blog.csdn.net/weixin_43069755/article/details/88209967

namespace SB {
    class StringBuilder;
}

class SkMemory : public SimpleMemory {

public:

    /**
     * one memory contains can multi table.
     * @param L
     * @param tableCount
     * @param minUnitSize
     */
    SkMemory(lua_State *L, int tableCount, const char *type);

    /**
     * create memory with default data.
     * @param type the data type
     * @param len the array length
     */
    SkMemory(const char *type, int len);

    //startIndex: table start index, from 0 means lua stack from 1
    SkMemory(lua_State *L, int startIndex, int tableCount, const char *type);

    //
    SkMemory();


    //------ index start from 0 --------

    bool isFloat();

    void toString(SB::StringBuilder &sb);

    /**
     * read data from memory to lua stack
     */
    static int read(SkMemory *mem, lua_State *L);

    /**
    * write data from lua stack to memory
    */
    static int write(SkMemory *mem, lua_State *L);

    int getLength();

    /**
     * write a data to dst memory
     * @param dstMem
     * @param dstIndex
     * @param srcIndex
     */
    void writeTo(SkMemory *dstMem, int dstIndex, int srcIndex);

    int convert(lua_State *L, const char *t);

    //for each with function call
    int foreach(lua_State *L);

    SkMemory* _mul(double val);
    SkMemory* dot(SkMemory* val);

    SkMemory *copy();

public:
    const char *_dType;
private:
    static int getTotalBytes(lua_State *L, int tableCount, const char *t);
};

/**
 * any types memory. a continuously space. this can contains multi table.
 */
class SkAnyMemory : public SimpleMemory {

public:
    /**
     * create a memory by multi table. every table has 'like' 'types' data. for example.
     * 'dfb' -> every table data can be [1, 2.0, 1] or [1, 2.0, 1, 1, 2.0, 1]... etc
     * @param L the lua stack
     * @param types the types.
     */
    SkAnyMemory(lua_State *L, const char *types);

    /**
     * create any memory byte types with table index.
     * @param L the lua stack
     * @param types the types
     * @param tableIndex the table index, if -1 means may be multi table
     */
    SkAnyMemory(lua_State *L, const char *types, int tableIndex);

    /**
    * create empty a memory with fix count length, every table has fixes 'types' data.
    * @param types the types.
    * @param count the table count.
    */
    SkAnyMemory(const char *types, int count);

    /**
     * @param types
     * @param count
     * @param init true to init data
     */
    SkAnyMemory(const char *types, int count, bool init);

    void toString(SB::StringBuilder &sb);

    SkAnyMemory *copy();

    int convert(lua_State *L, const char *t);

    int foreach(lua_State *L);

    SkAnyMemory* _mul(double val);

    int getLength() { return _tabCount * _elementCount; }

    static int read(SkAnyMemory *mem, lua_State *L);

    static int write(SkAnyMemory *mem, lua_State *L);

public:
    const char *_types;
    uint16_t _tabCount;      //table count.
    uint16_t _elementCount;  //element count of every table
};
//2
class SkMemoryMatrix : public IMemory {
public:
    ~SkMemoryMatrix();

    /** create a empty mat for latter use*/
    SkMemoryMatrix();

    /** internal use */
    SkMemoryMatrix(int count, bool singleType);

    /**
     * create memory matrix from multi table values which is from lua_state
     * @param L lua stack
     * @param type
     */
    SkMemoryMatrix(lua_State *L, const char *type);

    /**
     * create empty memory matrix.
     * @param types the types
     * @param rowCount the row count
     * @param columnCount the column count
     */
    SkMemoryMatrix(const char *types, int rowCount, int columnCount);

    void destroyData();

    bool isValid();

    inline int getLength() { return count; }

    int getRowCount() { return count; }

    int getColumnCount();

    const char *getTypes();

    /**
    * indicate the base memory is 'SkMemory' or not.
    * @return true if is single type memory
    */
    bool isSingleType();

    void toString(SB::StringBuilder &sb);

    SkMemoryMatrix* _mul(double val);
    SkMemoryMatrix *copy();

    SkMemoryMatrix *transpose();

    /**
     * convert every memory to target types
     * @param t  the types
     * @return the new matrix.
     */
    int convert(lua_State *L, const char *t);

    /**
     * travel the matrix with function.
     * @param L the lua state
     * @return the result state
     */
    int foreach(lua_State *L);

    /**
     * tiled the mat to normal memory.
     * @param L the lua stack
     * @return the result state of lua
     */
    int tiled(lua_State *L);

    static int read(SkMemoryMatrix *mem, lua_State *L, void (*Push)(lua_State *L, SkMemory *ptr));

    static int write(SkMemoryMatrix *mem, lua_State *L, SkMemory *(*Pull)(lua_State *L, int idx));

    static int
    read(SkMemoryMatrix *mem, lua_State *L, void (*Push)(lua_State *L, SkAnyMemory *ptr));

    static int
    write(SkMemoryMatrix *mem, lua_State *L, SkAnyMemory *(*Pull)(lua_State *L, int idx));

private:
    SkMemoryMatrix(int count);

    //unsigned char _isArray; // false for mat. true for special array
    int count;

    //only used for single type
    void copyData(SkMemory *pMemory, int columnIndex);

public:
    SkMemory **array;
    SkAnyMemory **anyArray;
};

#endif //BGFX_STUDY_SKMEMORY_H
