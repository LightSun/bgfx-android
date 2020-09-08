//
// Created by heaven7 on 2020/8/27 0027.
//

#ifndef BGFX_STUDY_SKMEMORY_H
#define BGFX_STUDY_SKMEMORY_H

#include "lua.hpp"
#include "IMemory.h"

class SkMemory;
class SkMemoryFFFUI;
class SkAnyMemory;
class SkMemoryMatrix;
namespace SB{
    class StringBuilder;
}

class SkMemory : public SimpleMemory{

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

    bool isFloat();
    void toString(SB::StringBuilder& sb);

    static int read(SkMemory* mem, lua_State* L);
    static int write(SkMemory* mem, lua_State* L);

    int getLength();

public:
    const char * _dType;
private:
    static int getTotalBytes(lua_State *L, int tableCount, const char *t);
};

/**
 * any types memory. a continuously space. this can contains multi table.
 */
class SkAnyMemory : public SimpleMemory{

public:
    /**
     * create a memory by multi table. every table has fixes 'types' data.
     * @param L the lua stack
     * @param types the types.
     */
    SkAnyMemory(lua_State* L,const char* types);
    /**
    * create empty a memory with fix count length, every table has fixes 'types' data.
    * @param types the types.
    * @param count the table count.
    */
    SkAnyMemory(lua_State *L,const char* types, int count);

    void toString(SB::StringBuilder& sb);

    int getLength(){return  _tabCount * _tabSize;}

public:
    uint16_t _tabCount; //table count.
    uint16_t _tabSize;  //table size
    const char* _types;

//private: uint8_t* tabSpace;// like dwbf: [4,2,1,4]
};

//float,float,float, uint32_t
class SkMemoryFFFUI : public SimpleMemory{
public:
    SkMemoryFFFUI(lua_State* L, int tableCount);

    int getLength();
    void toString(SB::StringBuilder& sb);

    static int read(SkMemoryFFFUI* mem, lua_State* L);
    static int write(SkMemoryFFFUI* mem, lua_State* L);
};
class SkMemoryMatrix: public IMemory{
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

    bool isValid() ;

    inline int getLength() { return count;}
    int getRowCount(){ return count; }
    int getColumnCount();
    void toString(SB::StringBuilder& sb);

    static int read(SkMemoryMatrix* mem, lua_State* L, void (*Push)(lua_State* L, SkMemory* ptr));
    static int write(SkMemoryMatrix* mem, lua_State* L, SkMemory* (*Pull)(lua_State* L, int idx));

private:
    SkMemory** array;
    int count;
};

#endif //BGFX_STUDY_SKMEMORY_H
