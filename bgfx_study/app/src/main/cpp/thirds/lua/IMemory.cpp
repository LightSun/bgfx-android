//
// Created by Administrator on 2020/9/3 0003.
//
#include <log.h>
#include "IMemory.h"
#include "stringbuilder.h"

#define FUNC_TO_STRING(type) \
const char * type::toString() { \
SB::StringBuilder* ss = new SB::StringBuilder(); \
toString(*ss); \
auto result = ss->toString(); \
delete(ss); \
return result; \
}
FUNC_TO_STRING(IMemory)


int MemoryUtils::getUnitSize(char t) {
    switch (t) {
        case 'f': //float
        case 'd': //unsigned int
            return 4;
        case 'w': //unsigned short
            return 2;
        case 'b': //unsigned char
            return 1;

        case 's': //short
            return 2;
        case 'i': //int
            return 4;
        case 'c': //char
            return 1;

        case 'F': //double
            return 8;

        default:
            return 0;
    }
}
#define UN_SIGNED_ADDR(data, offset) \
uint8_t * addr = static_cast<uint8_t *>(data);\
addr += offset;
#define SIGNED_ADDR(data, offset) \
signed char * addr = static_cast<signed char *>(data);\
addr += offset;

void MemoryUtils::toString(SB::StringBuilder &sb, char t, void *data, int totalIndex) {
    switch (t) {
        case 'f': {
            SIGNED_ADDR(data, totalIndex)
            sb << *(float*)addr;
        }break;

        case 'd':{
            UN_SIGNED_ADDR(data, totalIndex)
            sb << *(uint32_t *)addr;
        }
        break;

        case 'w':{
            UN_SIGNED_ADDR(data, totalIndex)
            sb << *(uint16_t *)addr;
        }
        break;

        case 'b':{
            UN_SIGNED_ADDR(data, totalIndex)
            sb << *(uint8_t *)addr;
        }
        break;

        case 's':{
            SIGNED_ADDR(data, totalIndex)
            sb << *(short *)addr;
        }
        break;
        case 'i':{
            SIGNED_ADDR(data, totalIndex)
            sb << *(int *)addr;
        }
        break;
        case 'c':{
            SIGNED_ADDR(data, totalIndex)
            sb << *(signed char *)addr;
        }
        break;

        case 'F':{
            SIGNED_ADDR(data, totalIndex)
            sb << *(double *)addr;
        }
            break;

        default:
            LOGE("wrong type = %c", t);
    }
}
int MemoryUtils::read(lua_State *L, char t, void *data, int totalIndex) {
    switch (t) {
        case 'f': {
            SIGNED_ADDR(data, totalIndex)
            *(float*)addr =static_cast<float>(lua_tonumber(L, -1));
        }
            break;
        case 'd':{
            UN_SIGNED_ADDR(data, totalIndex)
            *(uint32_t*)addr = static_cast<uint32_t >(lua_tointeger(L, -1));
        }break;
        case 'w':{
            UN_SIGNED_ADDR(data, totalIndex)
            *(uint16_t*)addr = static_cast<uint16_t >(lua_tointeger(L, -1));
        }break;
        case 'b':{
            UN_SIGNED_ADDR(data, totalIndex)
            *addr = static_cast<uint8_t >(lua_tointeger(L, -1));
        }break;

        case 's':{
            SIGNED_ADDR(data, totalIndex)
            *(short*)addr = static_cast<short>(lua_tointeger(L, -1));
        }break;
        case 'i':{
            SIGNED_ADDR(data, totalIndex)
            *(int*)addr = static_cast<int >(lua_tointeger(L, -1));
        }break;
        case 'c':{
            SIGNED_ADDR(data, totalIndex)
            *(signed char*)addr = static_cast<signed char>(lua_tointeger(L, -1));
        }break;

        case 'F':{
            SIGNED_ADDR(data, totalIndex)
            *(double*)addr = static_cast<double>(lua_tonumber(L, -1));
        }break;

        default:
            luaL_error(L, "unknown type of %c", t);
            return -1;
    }
    return 0;
}
int MemoryUtils::write(lua_State *L, char t, void *data, int totalIndex) {
    switch (t) {
        case 'f': {
            SIGNED_ADDR(data, totalIndex)
            lua_pushnumber(L, *(float*)addr);
        }
            break;
        case 'd':{
            UN_SIGNED_ADDR(data, totalIndex)
            lua_pushnumber(L, *(uint32_t *)addr);
        }break;
        case 'w':{
            UN_SIGNED_ADDR(data, totalIndex)
            lua_pushnumber(L, *(uint16_t *)addr);
        }break;
        case 'b':{
            UN_SIGNED_ADDR(data, totalIndex)
            lua_pushnumber(L, *addr);
        }break;

        case 's':{
            SIGNED_ADDR(data, totalIndex)
            lua_pushnumber(L, *(short *)addr);
        }break;
        case 'i':{
            SIGNED_ADDR(data, totalIndex)
            lua_pushnumber(L, *(int *)addr);
        }break;
        case 'c':{
            SIGNED_ADDR(data, totalIndex)
            lua_pushnumber(L, *(signed char *)addr);
        }break;
        case 'F':{
            SIGNED_ADDR(data, totalIndex)
            lua_pushnumber(L, *(double *)addr);
        }break;

        default:
            luaL_error(L, "unknown type of %c", t);
            return -1;
    }
    return 0;
}
void MemoryUtils::init(char t, void *data, int totalIndex) {
    switch (t) {
        case 'f': {
            SIGNED_ADDR(data, totalIndex)
            *(float*)addr = 0;
        }
            break;
        case 'd':{
            UN_SIGNED_ADDR(data, totalIndex)
            *(uint32_t *)addr = 0;
        }break;
        case 'w':{
            UN_SIGNED_ADDR(data, totalIndex)
            *(uint16_t *)addr = 0;
        }break;
        case 'b':{
            UN_SIGNED_ADDR(data, totalIndex)
            *addr = 0;
        }break;

        case 's':{
            SIGNED_ADDR(data, totalIndex)
            *(short *)addr = 0;
        }break;
        case 'i':{
            SIGNED_ADDR(data, totalIndex)
            *(int *)addr = 0;
        }break;
        case 'c':{
            SIGNED_ADDR(data, totalIndex)
            *(signed char*)addr = 0;
        }break;
        case 'F':{
            SIGNED_ADDR(data, totalIndex)
            *(double *)addr = 0;
        }break;

        default:
            LOGW("unknown type data to init. %c", t);
    }
}

#define SRC_ARR(type) type *srcArr = static_cast<type *>(srcData)
#define CAST_DATA(dst) \
{ \
dst *dstArr = static_cast<dst *>(dstData); \
srcArr += index;  \
dstArr += index; \
* dstArr = (dst)(*srcArr); }

#define CAST_DATA_DST() \
switch (dstType){ \
case 'f': \
CAST_DATA(float); \
break; \
case 'd': \
CAST_DATA(uint32_t) \
break; \
case 'w': \
CAST_DATA(uint16_t) \
break; \
case 'b': \
CAST_DATA(uint8_t) \
break; \
\
case 'i': \
CAST_DATA(int)\
break;\
case 's':\
CAST_DATA(short)\
break;\
case 'c':\
CAST_DATA(signed char)\
break;\
case 'F':\
CAST_DATA(double)\
break;\
}

void MemoryUtils::convert(void *srcData, const char srcType, void *dstData, const char dstType, int index) {
    switch (srcType) {
        case 'f': {
            SRC_ARR(float);
            CAST_DATA_DST();
        }
            break;

        case 'd': {
            SRC_ARR(uint32_t);
            CAST_DATA_DST();
        }
            break;
        case 'w': {
            SRC_ARR(uint16_t);
            CAST_DATA_DST();
        }
            break;
        case 'b': {
            SRC_ARR(uint8_t);
            CAST_DATA_DST();
        }
            break;

        case 'i': {
            SRC_ARR(int);
            CAST_DATA_DST();
        }
            break;
        case 's': {
            SRC_ARR(short);
            CAST_DATA_DST();
        }
            break;
        case 'c': {
            SRC_ARR(signed char);
            CAST_DATA_DST();
        }
            break;
        case 'F': {
            SRC_ARR(double);
            CAST_DATA_DST();
        }
            break;
    }
}

#define SRC_ADDR(type) type* srcArr = (type*)srcAddr
#define DST_DATA_ANY() \
switch (dstType){ \
case 'f': \
*(float*)dstAddr = (float)(*srcArr);\
break; \
case 'd': \
*(uint32_t*)dstAddr = (uint32_t)(*srcArr); \
break; \
case 'w': \
*(uint16_t*)dstAddr = (uint16_t)(*srcArr); \
break; \
case 'b': \
*(uint8_t*)dstAddr = (uint8_t)(*srcArr); \
break; \
\
case 'i': \
*(int*)dstAddr = (int)(*srcArr); \
break;\
case 's':\
*(short*)dstAddr = (short)(*srcArr); \
break;\
case 'c':\
*(signed char*)dstAddr =  (signed char)(*srcArr); \
break;\
case 'F':\
*(double*)dstAddr =  (double)(*srcArr); \
break;\
}

void MemoryUtils::convert(void *srcData, char srcType, size_t srcBytes,
        void *dstData, const char dstType, size_t dstBytes) {
    uint8_t * srcAddr = static_cast<uint8_t *>(srcData);
    uint8_t * dstAddr = static_cast<uint8_t *>(dstData);
    srcAddr += srcBytes;
    dstAddr += dstBytes;

    switch (srcType) {
        case 'f': {
            SRC_ADDR(float);
            DST_DATA_ANY();
        }
            break;

        case 'd': {
            SRC_ADDR(uint32_t);
            DST_DATA_ANY();
        }
            break;
        case 'w': {
            SRC_ADDR(uint16_t);
            DST_DATA_ANY();
        }
            break;
        case 'b': {
            SRC_ADDR(uint8_t);
            DST_DATA_ANY();
        }
            break;

        case 'i': {
            SRC_ADDR(int);
            DST_DATA_ANY();
        }
            break;
        case 's': {
            SRC_ADDR(short);
            DST_DATA_ANY();
        }
            break;
        case 'c': {
            SRC_ADDR(signed char);
            DST_DATA_ANY();
        }
            break;
        case 'F': {
            SRC_ADDR(double);
            DST_DATA_ANY();
        }
            break;
    }
}

#define OP(type, val, _op) \
type* arr = (type*)addr; \
*arr = static_cast<type>((*arr) _op val);

void MemoryUtils::multiple(void *srcData, const char type, size_t totalIndex, double val) {
    switch (type){
        case 'f': {
            SIGNED_ADDR(srcData, totalIndex);
            OP(float, val, *);
        }
            break;
        case 'd':{
            UN_SIGNED_ADDR(srcData, totalIndex);
            OP(uint32_t , val, *);
        }
            break;

        case 'w':{
            UN_SIGNED_ADDR(srcData, totalIndex);
            OP(uint16_t, val, *);
        }
            break;
        case 'b':{
            UN_SIGNED_ADDR(srcData, totalIndex);
            OP(uint8_t, val, *);
        }
            break;

        case 'i':{
            SIGNED_ADDR(srcData, totalIndex);
            OP(int, val, *);
        }
            break;
        case 's':{
            SIGNED_ADDR(srcData, totalIndex);
            OP(short, val, *);
        }
            break;
        case 'c':{
            SIGNED_ADDR(srcData, totalIndex);
            OP(s_char, val, *);
        }
            break;
        case 'F':{
            SIGNED_ADDR(srcData, totalIndex);
            OP(double, val, *);
        }
            break;
    }
}

void MemoryUtils::multiple(void *srcData, const char type, size_t totalIndex, lua_Integer val) {
    switch (type){
        case 'f': {
            SIGNED_ADDR(srcData, totalIndex);
            OP(float, val, *);
        }
            break;
        case 'd':{
            UN_SIGNED_ADDR(srcData, totalIndex);
            OP(uint32_t , val, *);
        }
            break;

        case 'w':{
            UN_SIGNED_ADDR(srcData, totalIndex);
            OP(uint16_t, val, *);
        }
            break;
        case 'b':{
            UN_SIGNED_ADDR(srcData, totalIndex);
            OP(uint8_t, val, *);
        }
            break;

        case 'i':{
            SIGNED_ADDR(srcData, totalIndex);
            OP(int, val, *);
        }
            break;
        case 's':{
            SIGNED_ADDR(srcData, totalIndex);
            OP(short, val, *);
        }
            break;
        case 'c':{
            SIGNED_ADDR(srcData, totalIndex);
            OP(s_char, val, *);
        }
            break;
        case 'F':{
            SIGNED_ADDR(srcData, totalIndex);
            OP(double, val, *);
        }
            break;
    }
}

//---------------------------------------
IMemory::IMemory():_ref(1) {

}
int IMemory::ref() {
    return _ref.fetch_add(1);
}
int IMemory::unRef() {
    return _ref.fetch_add(-1);
}

//-------------------------------------
SimpleMemory::SimpleMemory():IMemory() {

}
SimpleMemory::~SimpleMemory(){
    destroyData();
}
void SimpleMemory::destroyData() {
    if(data){
        free(data);
        data = nullptr;
    }
}

bool SimpleMemory::isValid() {
    return data != nullptr;
}