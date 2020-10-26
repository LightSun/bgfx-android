//
// Created by heaven7 on 2020/8/27 0027.
// currently for memory data. support types are: d(uint32), w(uint16), b(uint8),f(float),
//                                               i(int),  s(short), c(char), F(double)
//

#ifndef BGFX_STUDY_SKMEMORY_H
#define BGFX_STUDY_SKMEMORY_H

#include "lua.hpp"
#include "IMemory.h"
#define DEF_RESHAPE_TYPE 0  //default unknown type

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

    ~SkMemory();
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

    SkMemory(const char *type, int len, bool init);

    //startIndex: table start index, from 0 means lua stack from 1
    SkMemory(lua_State *L, int startIndex, int tableCount, const char *type);

    //
    SkMemory();

    void destroyData();

    bool isFloat();

    void toString(SB::StringBuilder &sb);

    const char *getTypes(){ return _types;}

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
    SkMemory* _mul(SkMemory* val);
    SkMemory* _mul(SkAnyMemory* val);

    /**
     * multiple the matrix like 'dot-mat' or not. for dotMatrix this behaviour just like numpy.
     * @param val the mat
     * @param dotMat true if multi like dot-mat.
     * @return the result memory
     */
    SkMemory* _mul(SkMemoryMatrix* val, bool dotMat = true);

    SkMemory* dot(SkMemoryMatrix* val);

    SkMemory *copy();

    SkMemory *extract(size_t start, size_t end1);

    static SkMemory* create(const char* type, int count);
    static SkMemory* create(char type, int count);

    /** kick out the index value from memory data.*/
    SkMemory *kickOut(size_t index);

    bool equals(SkMemory* o);

    /**
     * reshape the memory data.
     * @param count the expect count of element
     * @param t the data type to result memory
     * @param defVal the default value if not enough
     * @return the result memory
     */
    SkMemory* reshape(int count, char t = DEF_RESHAPE_TYPE, double defVal = 0);


   //SkMemoryMatrix* reshapeMat(const SkMemory* oth, char t = DEF_RESHAPE_TYPE, double defVal = 0);
    /**
     * relative to reshape. just add default value first then add current data.
     * @param count the result count
     * @param t the result type
     * @param defVal the default value
     * @return the result memory
     */
    SkMemory* reshapeBefore(int count, char t = DEF_RESHAPE_TYPE, double defVal = 0);

    /**
     * concat the memory
     * @param pMemory the target memory to concat/merge
     * @param resultCount the result element count
     * @param resultType the result data type of memory
     * @param defVal the default value if not enough
     * @return the result memory
     */
    SkMemory *concat(SkMemory *pMemory, int resultCount, char resultType = DEF_RESHAPE_TYPE, double defVal = 0);
    /**
      * concat the memory
      * @param pMemory the target memory to concat/merge
      * @param resultCount the result element count
      * @param resultType the result data type of memory
      * @param defVal the default value if not enough
      * @return the result memory
      */
    SkMemory *concat(SkAnyMemory *pMemory, int resultCount, char resultType = DEF_RESHAPE_TYPE, double defVal = 0);

    /**
     * flip the memory data
     * @param copy true if result as copy
     * @return the result memory
     */
    SkMemory *flip(bool copy = true);

    /**
     * like matlib diag for build mat.
     * @param k the k index
     * @param defVal the default value
     * @return the mat
     */
    SkMemoryMatrix* diag(int k = 0, double defVal = 0);

    //------ index start from 0 --------
    bool setValue(int index, double val);
    bool getValue(int index, double* result);
public:
    const char *_types;
private:
    char _needFreePtr = 0;
    static int getTotalBytes(lua_State *L, int tableCount, const char *t);

    void swapValue(int index1, int index2);
};

/**
 * any types memory. a continuously space. this can contains multi table.
 */
class SkAnyMemory : public SimpleMemory {

public:
    const char *_types;
    uint16_t _tabCount;      //table count.
    uint16_t _elementCount;  //element count of every table

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
    SkAnyMemory(const char *types, int tabCount);

    /**
     * @param types
     * @param count the table count
     * @param init true to init data
     */
    SkAnyMemory(const char *types, int tabCount, bool init);

    void toString(SB::StringBuilder &sb);

    const char *getTypes(){ return _types;}
    //----------------------------------------------

    SkAnyMemory *copy();

    int convert(lua_State *L, const char *t);

    int foreach(lua_State *L);

    SkAnyMemory* _mul(double val);
    SkAnyMemory* _mul(SkMemory* val);
    SkAnyMemory* _mul(SkAnyMemory* val);

    SkMemory* _mul(SkMemoryMatrix* val, bool dotMat = true);

    SkMemory* dot(SkMemoryMatrix* val);

    SkMemory *extract(size_t start, size_t end1);

    SkMemory *reshape(int count, char t = DEF_RESHAPE_TYPE, double defVal = 0);
    SkMemory *reshapeBefore(int count, char t = DEF_RESHAPE_TYPE, double defVal = 0);

    SkMemory *concat(SkMemory *pMemory, int resultCount, char type = DEF_RESHAPE_TYPE, double defVal = 0);
    SkMemory *concat(SkAnyMemory *pMemory, int resultCount, char type = DEF_RESHAPE_TYPE, double defVal = 0);

    SkMemory *flip();
    SkMemoryMatrix* diag(int k = 0, double defVal = 0);

    int getLength() { return _tabCount * _elementCount; }

    SkMemory *kickOut(size_t index);

    bool equals(SkAnyMemory* o);

    bool setValue(int index, double val);
    bool getValue(int index, double* result);
};
//2
class SkMemoryMatrix : public IMemory {

private:
    SkMemoryMatrix(int count);

    //unsigned char _isArray; // false for mat. true for special array
    int count;

    /** collect column data to array. */
    void copyData(SkMemory *pMemory, int columnIndex);

    void swapValue(int rowIndex1, int colIndex1, int rowIndex2, int colIndex2);

    //Upper triangular matrix / Lower triangular matrix
    SkMemoryMatrix* _triul(bool up,int k = 0, double defVal = 0);
public:
    SkMemory **array;
    SkAnyMemory **anyArray;

    static int read(SkMemoryMatrix *mem, lua_State *L, void (*Push)(lua_State *L, SkMemory *ptr));

    static int write(SkMemoryMatrix *mem, lua_State *L, SkMemory *(*Pull)(lua_State *L, int idx));

    static int
    read(SkMemoryMatrix *mem, lua_State *L, void (*Push)(lua_State *L, SkAnyMemory *ptr));

    static int
    write(SkMemoryMatrix *mem, lua_State *L, SkAnyMemory *(*Pull)(lua_State *L, int idx));

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

    inline bool isSquareMatrix(){ return getRowCount() == getColumnCount();}

    /** get the row types */
    const char *getTypes();

    /**
    * indicate the base memory is 'SkMemory' or not.
    * @return true if is single type memory
    */
    bool isSingleType();

    void toString(SB::StringBuilder &sb);

    SkMemoryMatrix* _mul(double val);
    //current mat must be one column.
    SkMemoryMatrix* _mul(SkMemory* val);
    SkMemoryMatrix* _mul(SkAnyMemory* val);

    SkMemoryMatrix* _mul(SkMemoryMatrix* val);

    SkMemoryMatrix *copy();

    SkMemoryMatrix *transpose();

    double determinant();                                                //行列式的值
    double remainderValue(size_t rowIndex, size_t columnIndex);          //代数余子式值
    SkMemoryMatrix* remainderMat(size_t rowIndex, size_t columnIndex);   //余子式矩阵
    SkMemoryMatrix* algebraicRemainderMat();     //代数余子式矩阵.
    SkMemoryMatrix* adjointMat();                //Adjoint matrix 伴随矩阵
    /**
    * extract a column data to a row as SkMemory
    * @param columnIndex the column index
    * @param out the out memory. can be null
    * @return the out memory
    */
    SkMemory* extractColumn(int columnIndex, SkMemory* out);
    /**
     * extract sub matrix
     * @param rowStart  the row start index .include
     * @param rowEnd  the row end index .exclude
     * @param columnStart the column start index .include
     * @param columnEnd  the column end index .exclude
     * @return result mat
     */
    SkMemoryMatrix* extractMat(size_t rowStart, size_t rowEnd, size_t columnStart, size_t columnEnd);

    /**
     * reshape a mat to another.
     * @param rowCount the expect row count
     * @param colCount the expect column count
     * @param defVal the default value default is 0
     * @param type the expect type ,default is float
     * @return the result mat.
     */
    SkMemoryMatrix* reshape(int rowCount, int colCount, char type = DEF_RESHAPE_TYPE, double defVal = 0);

    /**
     * concat target mat by vertical or horizontal. And current mat is the left top.
     * @param oth the target mat to merge
     * @param vertical vertical or horizontal
     * @param defVal default value . if element not enough
     * @return the result mat
     */
    SkMemoryMatrix* concat(SkMemoryMatrix* oth, bool vertical = true, double defVal = 0);

    /**
     * flip the left right data.
     * @param changeRawData true if change raw data. if is SkAnyMemory. this will be ignore.
     * @return the mat
     */
    SkMemoryMatrix* fliplr(bool copy = true);
    /**
       * flip the up down data.
       * @return the mat
       */
    SkMemoryMatrix* flipud(bool copy = true);

    SkMemory* diag(int k = 0);

    SkMemoryMatrix* triu(int k = 0, double defVal = 0);

    SkMemoryMatrix* tril(int k = 0, double defVal = 0);
    /**
     * inverse the matrix
     * @param L the lua state
     * @return 1 if success, 0 otherwise.
     */
    int inverse(lua_State *L);
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

    bool getValue(int rowIndex, int colIndex, double* result);
    bool setValue(int rowIndex, int colIndex, double val);

    bool equals(SkMemoryMatrix* o);
};

#endif //BGFX_STUDY_SKMEMORY_H
