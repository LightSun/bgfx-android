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

bool MemoryUtils::isSigned(const char t) {
    switch (t) {
        case 'd': //unsigned int
        case 'w': //unsigned short
        case 'b': //unsigned char
            return false;

        case 'f':
        case 's': //short
        case 'i': //int
        case 'c': //char
        case 'F': //double
            return true;

        default:
            return true;
    }
}

#define UN_SIGNED_ADDR(data, offset) \
uint8_t * addr = static_cast<uint8_t *>(data);\
addr += offset;
#define SIGNED_ADDR(data, offset) \
signed char * addr = static_cast<signed char *>(data);\
addr += offset;

double MemoryUtils::getValue(void *data, const char t, size_t bytesIndex) {
#define GET_VAL_SIGN(type, data,offset) {\
signed char * addr = static_cast<signed char *>(data);\
addr += offset; \
return *(type *)addr;}

#define GET_VAL_UNSIGN(type, data,offset) \
{ \
    unsigned char *addr = static_cast<unsigned char *>(data);\
addr += offset; \
return  *(type *) addr;}

    switch (t) {
        case 'd':
            GET_VAL_UNSIGN(uint32_t, data, bytesIndex)
        case 'w':
            GET_VAL_UNSIGN(uint16_t, data, bytesIndex)
        case 'b':
            GET_VAL_UNSIGN(uint8_t, data, bytesIndex)

        case 'f':
            GET_VAL_SIGN(float, data, bytesIndex)

        case 's': //short
            GET_VAL_SIGN(short, data, bytesIndex)
        case 'i': //int
            GET_VAL_SIGN(int, data, bytesIndex)
        case 'c': //char
            GET_VAL_SIGN(char, data, bytesIndex)
        case 'F': //double
            GET_VAL_SIGN(double, data, bytesIndex)
        default:
            LOGE("wrong type = %d", t);
            return 0;
    }
}

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

#define OP_TO_DATA(type, sn ,op) \
{  \
auto v1 = getValue(srcData, srcType, srcBytes); \
auto v2 = getValue(dstData, dstType, dstBytes); \
if(sn){ \
auto addr = static_cast<signed char*>(outData); \
addr += outBytes; \
*(type*)addr = (type)(v1 op v2); \
}else{ \
auto addr = static_cast<unsigned char*>(outData); \
addr += outBytes; \
*(type*)addr = (type)(v1 op v2); \
} \
}

#define OP_SRC_DST_OUT(name, op) \
void MemoryUtils::name(void *srcData, const char srcType, size_t srcBytes, \
        void *dstData, const char dstType, size_t dstBytes, \
        void *outData, const char outType, size_t outBytes) { \
    switch (outType){ \
        case 'f': \
            OP_TO_DATA(float, true, op) \
            break; \
        case 'd': \
            OP_TO_DATA(uint32_t , false, op) \
            break; \
        case 'w': \
            OP_TO_DATA(uint16_t, false, op) \
            break; \
        case 'b': \
            OP_TO_DATA(uint8_t, false, op) \
            break; \
        case 'i': \
            OP_TO_DATA(int, true, op) \
            break; \
        case 's': \
            OP_TO_DATA(short, true, op) \
            break; \
        case 'c': \
            OP_TO_DATA(s_char, true, op) \
            break; \
        case 'F': \
            OP_TO_DATA(double , true, op) \
            break;\
    } \
}
OP_SRC_DST_OUT(multiple, *)

double MemoryUtils::multiple(void *srcData, const char type, size_t totalIndex, void *dstData,
                             const char dstType, size_t dstBytes) {
    return getValue(srcData, type, totalIndex) * getValue(dstData, dstType, dstBytes);
}

const char MemoryUtils::computeType(const char *ts) {
    char type = ts[0];
    for (int i = 1, len = strlen(ts); i < len; ++i) {
        type = MemoryUtils::computeType(type, ts[i]);
    }
    return type;
}
const char MemoryUtils::computeType(const char type, const char type1) {
    if(type == type1){
        return type;
    }
    const bool isFloat_t = type == 'f' || type == 'F';
    const bool isFloat_t1 = type1 == 'f' || type1 == 'F';

    //isFloat_t ? type : (isFloat_t1 ? upgradeType(type, true) : type): used to return a float-signed type.
    if(getUnitSize(type) > getUnitSize(type1)){
        if(isSigned(type)) return isFloat_t ? type : (isFloat_t1 ? upgradeType(type, true) : type);
        else{
            if(isSigned(type1)) return upgradeType(type, isFloat_t1);
            else{
                return type;
            }
        }
    } else if(getUnitSize(type) == getUnitSize(type1)){
        if(isSigned(type)){
            if(isSigned(type1)){
                return isFloat_t ? type : type1;
            } else{
                return upgradeType(type1, isFloat_t);
            }
        } else{
            if(isSigned(type1)){
                return upgradeType(type, isFloat_t1);
            } else{
                return type; //no signed means no float
            }
        }
    } else{
        if(isSigned(type1)) return isFloat_t1 ? type1 : (isFloat_t ? upgradeType(type, true) : type1);
        else{
            if(isSigned(type)) return upgradeType(type1, isFloat_t);
            else{
                return type1; //no signed means no float
            }
        }
    }
}
//type is unsigned
const char MemoryUtils::upgradeType(const char type, const bool hasFloat) {
    switch (type){
        case 'd':
            return 'F';

        case 'w':{
            return hasFloat ? 'f' : 'i';
        }
        case 'b':{
            return hasFloat ? 'f' : 's';
        }
    }
    LOGE("can't reach here for MemoryUtils::upgradeType()");
    //can't reach here
    return 'i';
}

double MemoryUtils::pile(void *data1, const char type1, void *data2, const char type2, const int count) {
    const int size1 = getUnitSize(type1);
    const int size2 = getUnitSize(type2);
    double out = 0;
    for (int i = 0; i < count; ++i) {
        out += getValue(data1, type1, i * size1) * getValue(data2, type2, i * size2);
    }
    return out;
}
void MemoryUtils::setValue(void *data, const char t, size_t bytesIndex, double val) {
#define ASSIGN(type)  *(type*)addr = (type)val

    switch (t){
        case 'f': {
            SIGNED_ADDR(data, bytesIndex);
            ASSIGN(float);
        }
            break;
        case 'd':{
            UN_SIGNED_ADDR(data, bytesIndex);
            ASSIGN(uint32_t);
        }
            break;

        case 'w':{
            UN_SIGNED_ADDR(data, bytesIndex);
            ASSIGN(uint16_t);
        }
            break;
        case 'b':{
            UN_SIGNED_ADDR(data, bytesIndex);
            ASSIGN(uint8_t);
        }
            break;

        case 'i':{
            SIGNED_ADDR(data, bytesIndex);
            ASSIGN(int);
        }
            break;
        case 's':{
            SIGNED_ADDR(data, bytesIndex);
            ASSIGN(short);
        }
            break;
        case 'c':{
            SIGNED_ADDR(data, bytesIndex);
            ASSIGN(s_char);
        }
            break;
        case 'F':{
            SIGNED_ADDR(data, bytesIndex);
            ASSIGN(double);
        }
            break;
    }
}

const size_t MemoryUtils::computeBytesIndex(const char *types, size_t index) {
    size_t bytesIndex = 0;
    const size_t len = strlen(types);
    for (int i = 0; i < index; ++i) {
        bytesIndex += MemoryUtils::getUnitSize(types[i % len]);
    }
    return bytesIndex;
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
void IMemory::unRefAndDestroy() {
    if(unRef() == 0){
        destroyData();
        delete this;
    }
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