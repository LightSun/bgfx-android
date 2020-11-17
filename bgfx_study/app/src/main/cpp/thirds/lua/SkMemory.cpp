//
// Created by Administrator on 2020/8/27 0027.
//
#include <SkUTF.h>

#include "printer.h"
#include "SkMemory.h"
#include "common.h"
#include "LuaUtils.h"
#include "MathUtils.h"
#include "HMath.h"

#define copy_data_f(dType) \
dType * addr = static_cast<dType *>(data); \
for (int i = start; i < start + tableCount; ++i) { \
    idx = 1; \
    type = lua_rawgeti(L, i + 1, idx); \
    for (; type != LUA_TNIL ;) { \
        *addr = static_cast<dType>(lua_tonumber(L, -1)); \
        addr ++; \
        lua_pop(L, 1); \
        ++idx; \
        type = lua_rawgeti(L, i + 1, idx); \
    } \
}

#define copy_data_i(dType) \
dType * addr = static_cast<dType *>(data); \
for (int i = start; i < start + tableCount; ++i) { \
    idx = 1; \
    type = lua_rawgeti(L, i + 1, idx); \
    for (; type != LUA_TNIL ;) { \
        *addr = static_cast<dType>(lua_tointeger(L, -1)); \
        addr ++; \
        lua_pop(L, 1); \
        ++idx; \
        type = lua_rawgeti(L, i + 1, idx); \
    } \
}


#define ARRAY_INIT(type) \
type* arr = static_cast<type *>(data); \
for (int i = 0; i < len; ++i) { \
arr[i] = 0; \
}

#define COPY_SINGLE_DATA(type) \
{ auto pData = (type *) (dstMem->data); \
auto srcData = (type *) (data); \
pData += dstIndex; \
srcData += srcIndex; \
*pData = *srcData; }

//---------------------------------------------------------------------------------
SkMemory::SkMemory(): SimpleMemory(){

}
SkMemory::SkMemory(const char *type, int len, bool init): SimpleMemory(), _types(type){
    size = MemoryUtils::getUnitSize(type[0]) * len;
    SkASSERT(size > 0);
    data = malloc(size);
    if(init){
        switch (type[0]) {
            case 'f': {
                ARRAY_INIT(float);
            }
                break;
            case 'd': {
                ARRAY_INIT(uint32_t);
            }
                break;
            case 'w': {
                ARRAY_INIT(uint16_t);
            }
                break;
            case 'b':{
                ARRAY_INIT(uint8_t);
            }

            case 's':{
                ARRAY_INIT(short);
            }
            case 'i':{
                ARRAY_INIT(int);
            }
            case 'c':{
                ARRAY_INIT(s_char);
            }

            case 'F':{
                ARRAY_INIT(double);
            }
                break;
        }
    }
}
SkMemory::SkMemory(const char *type, int len): SkMemory(type, len ,true){
}
//start from 0
SkMemory::SkMemory(lua_State *L, int start, int tableCount, const char *t) : SimpleMemory(){
    _types = t;
    size = getTotalBytes(L, tableCount, t);
    SkASSERT(size > 0);
    data = malloc(size);
    int type;
    int idx;
    switch (t[0]) {
        case 'f': {
            copy_data_f(float);
        }
            break;
        case 'd': {
            copy_data_i(uint32_t);
        }
            break;
        case 'w': {
            copy_data_i(uint16_t);
        }
            break;
        case 'b':{
            copy_data_i(uint8_t);
        }
        break;

        case 's':{
            copy_data_i(short);
        } break;

        case 'i':{
            copy_data_i(int);
        } break;

        case 'c':{
            copy_data_i(s_char);
        } break;

        case 'F': {
            copy_data_f(double);
        }
            break;

        default:
            luaL_error(L, "wrong data type = %c", t[0]);
            return ;
    }
    lua_pop(L, 1);
}
void SkMemory::destroyData(){
    SimpleMemory::destroyData();
    if(_needFreePtr && _types != nullptr){
        free((void*)_types);
        _types = nullptr;
        _needFreePtr = 0;
    }
}
SkMemory::~SkMemory() {
    destroyData();
}
SkMemory::SkMemory(lua_State *L, int tableCount, const char *t) : SkMemory(L, 0, tableCount, t){
}

bool SkMemory::isFloat() {
    return _types[0] == 'f';
}

void SkMemory::toString(SB::StringBuilder& ss) {
    auto len = getLength();
    switch (_types[0]) {
        case 'f':
            Printer::printArray((float*)data, len, ss);
            break;
        case 'd':
            Printer::printArray((uint32_t*)data, len, ss );
            break;
        case 'w':
            Printer::printArray((uint16_t*)data, len, ss);
            break;
        case 'b':
            Printer::printArray((uint8_t*)data, len, ss);
            break;

        case 's':
            Printer::printArray((short *)data, len, ss);
            break;
        case 'i':
            Printer::printArray((int*)data, len, ss);
            break;
        case 'c':
            Printer::printArray((s_char*)data, len, ss);
            break;

        case 'F':
            Printer::printArray((double *)data, len, ss);
            break;
    }
    /* auto i = SkUTF::CountUTF8(result, sr.length());
     LOGD("%s :  utf8 count = %d", result, i);*/
}
//-----------------------------------------------------------------------
inline int SkMemory::getTotalBytes(lua_State *L, int tableCount, const char *t){
    int minSize = MemoryUtils::getUnitSize(t[0]);
    if(minSize == 0){
        luaL_error(L, "wrong type of memory-unit: %s", t);
        return 0;
    }
    int totalSize = 0 ;
    for (int i = 0; i < tableCount; ++i) {
        size_t len = lua_rawlen(L, i + 1);
        totalSize += len * minSize;
    }
    return totalSize;
}

int SkMemory::getLength() {
    return size / MemoryUtils::getUnitSize(_types[0]);
}

void SkMemory::writeTo(SkMemory *dstMem, int dstIndex, int srcIndex) {
    switch (_types[0]) {
        case 'f':
        COPY_SINGLE_DATA(float)
            break;
        case 'd':
        COPY_SINGLE_DATA(uint32_t)
            break;
        case 'w':
        COPY_SINGLE_DATA(uint16_t)
            break;
        case 'b':
        COPY_SINGLE_DATA(int8_t)
            break;

        case 's':
        COPY_SINGLE_DATA(short)
            break;
        case 'i':
        COPY_SINGLE_DATA(int)
            break;
        case 'c':
        COPY_SINGLE_DATA(s_char)
            break;

        case 'F':
        COPY_SINGLE_DATA(double)
            break;
    }
}
int SkMemory::convert(lua_State* L, const char* ts) {
    auto dstStrLen = strlen(ts);
    if(dstStrLen == 1){
        if(_types[0] == ts[0]){
            SkMemory *pSkMemory = LuaUtils::to_ref<SkMemory>(L, 1);
            if(pSkMemory != nullptr){
                lua_pushvalue(L, 1);
                return 1;
            }
            pSkMemory = LuaUtils::to_ref<SkMemory>(L, lua_upvalueindex(1));
            if(pSkMemory != nullptr){
                lua_pushvalue(L, 1);
                return 1;
            }
            return luaL_error(L, "wrong state for 'SkMemory::convert'");
        }
        auto pMemory = SkMemory::create(ts, getLength());
        for (int i = 0, length = getLength(); i < length; ++i) {
            MemoryUtils::convert(data, _types[0] , pMemory->data, ts[0], i);
        }
        LuaUtils::push_ptr(L, pMemory);
        return 1;
    }else{
        //error mismatch
        if(getLength() % dstStrLen != 0){
            return luaL_error(L, "length mismatch");
        }
        size_t tabCount = getLength() / dstStrLen;
        const auto pMemory = new SkAnyMemory(ts, tabCount, false);
        //copy data
        const char srcType = _types[0];
        const auto unitSize = MemoryUtils::getUnitSize(srcType);
        char dstType;
        size_t  dstBytes = 0;
        for (int i = 0, length = getLength(); i < length; ++i) {
            dstType = ts[i % dstStrLen];
            MemoryUtils::convert(data, srcType, i * unitSize, pMemory->data, dstType, dstBytes);
            dstBytes += MemoryUtils::getUnitSize(dstType);
        }
        LuaUtils::push_ptr(L, pMemory);
        return 1;
    }
}
int SkMemory::foreach(lua_State* L){
    //- 1 must be function.
    const char srcType = _types[0];
    const int unitSize = MemoryUtils::getUnitSize(srcType);
    for (int i = 0, length = getLength(); i < length; ++i) {
        lua_pushvalue(L, -1);
        lua_pushinteger(L, i);
        MemoryUtils::write(L, srcType, data, i * unitSize);
        if(lua_pcall(L, 2, 0, 0) != LUA_OK){
            return luaL_error(L, "call SkMemory.foreach(...) failed.");
        }
    }
    return 0;
}
SkMemory* SkMemory::copy() {
    auto pMemory = new SkMemory();
    pMemory->_types = _types;
    pMemory->size = size;
    pMemory->data = malloc(size);
    memcpy(pMemory->data, data, size);
    return pMemory;
}
SkMemory* SkMemory::_mul(double val) {
    SkMemory *pMemory = copy();
    const char type = _types[0];
    const int unitSize = MemoryUtils::getUnitSize(type);
    
    const bool isInt = floor(val) == val;
    for (int j = 0; j < getLength(); ++j) {
        if (isInt) {
            MemoryUtils::multiple(pMemory->data, type, j * unitSize, (lua_Integer) val);
        } else {
            MemoryUtils::multiple(pMemory->data, type, j * unitSize, val);
        }
    }
    return pMemory;
}
SkMemory* SkMemory::_mul(SkMemory *val) {
    if(getLength() != val->getLength()){
        return nullptr;
    }
    const char srcType = _types[0];
    const char dstType = val->_types[0];
    const char outType = MemoryUtils::computeType(srcType, dstType);
    const int srcUnitSize = MemoryUtils::getUnitSize(srcType);
    const int dstUnitSize = MemoryUtils::getUnitSize(dstType);
    const int outUnitSize = MemoryUtils::getUnitSize(outType);

    SkMemory *pMemory = SkMemory::create(outType, getLength());

    for (int j = 0; j < getLength(); ++j) {
        auto srcVal = MemoryUtils::getValue(data, srcType, j * srcUnitSize);
        auto dstVal = MemoryUtils::getValue(val->data, dstType, j * dstUnitSize);

        MemoryUtils::setValue(pMemory->data, outType, j * outUnitSize, srcVal * dstVal);
    }
    return pMemory;
}
SkMemory * SkMemory::_mul(SkAnyMemory *val) {
    if(getLength() != val->getLength()){
        return nullptr;
    }
    const char srcType = _types[0];
    const int srcUnitSize = MemoryUtils::getUnitSize(srcType);
    //compute result type
    const auto outType = MemoryUtils::computeType(MemoryUtils::computeType(val->_types), _types[0]);
    const int outUnitSize = MemoryUtils::getUnitSize(outType);

    auto out_mem = SkMemory::create(outType, getLength());

    const auto len_type = strlen(val->_types);
    char dstType;
    size_t dstBytes = 0;
    for (int j = 0; j < getLength(); ++j) {
        dstType = val->_types[j % len_type];

        auto srcVal = MemoryUtils::getValue(data, srcType, j * srcUnitSize);
        auto dstVal = MemoryUtils::getValue(val->data, dstType, dstBytes);
        MemoryUtils::setValue(out_mem->data, outType, j * outUnitSize, srcVal * dstVal);
        dstBytes += MemoryUtils::getUnitSize(dstType);
    }
    return out_mem;
}
SkMemory* SkMemory::_mul(SkMemoryMatrix *val, bool dotMat) {
    if(dotMat){
        return dot(val);
    }
    //length mus be equal
    if(getLength() != val->getLength()){
        return nullptr;
    }
    const int columnCount = val->getColumnCount();
    const int srcCount = getLength();
    const char srcType = _types[0];
    const int srcUnitSize = MemoryUtils::getUnitSize(srcType);

    const char outType = MemoryUtils::computeType(srcType, MemoryUtils::computeType(val->getTypes()));
    const int outUnitSize = MemoryUtils::getUnitSize(outType);
    auto out_mem = SkMemory::create(outType, srcCount);

    double value;
    double srcVal;
    if(val->isSingleType()){
        const char dstType = val->getTypes()[0];
        const int dstUnitSize = MemoryUtils::getUnitSize(dstType);
        SkMemory *mem_dst;
        for (int i = 0; i < srcCount; ++i) {
            mem_dst = val->array[i];
            value = 0;
            for (int j = 0; j < columnCount; ++j) {
                value += MemoryUtils::getValue(mem_dst->data, dstType, j* dstUnitSize);
            }
            srcVal = MemoryUtils::getValue(data, srcType, i * srcUnitSize);
            MemoryUtils::setValue(out_mem->data, outType, i * outUnitSize, srcVal * value);
        }
    } else{
        SkAnyMemory *mem_dst;
        char dstType;
        size_t dstType_len;
        size_t bytesIndex;
        for (int i = 0; i < srcCount; ++i) {
            mem_dst = val->anyArray[i];
            value = 0;
            dstType_len = strlen(mem_dst->getTypes());
            bytesIndex = 0;
            for (int j = 0; j < columnCount; ++j) {
                dstType = mem_dst->getTypes()[j % dstType_len];
                value += MemoryUtils::getValue(mem_dst->data, dstType, bytesIndex);
                bytesIndex += MemoryUtils::getUnitSize(dstType);
            }
            srcVal = MemoryUtils::getValue(data, srcType, i * srcUnitSize);
            MemoryUtils::setValue(out_mem->data, outType, i * outUnitSize, srcVal * value);
        }
    }
    return out_mem;
}
SkMemory* SkMemory::dot(SkMemoryMatrix *val) {
    //点积累加 eg: len = 2, mat = (3, 2)
    if(getLength() != val->getColumnCount()){
        return nullptr;//un support
    }
    //SkMemory (dot) SkMemoryMatrix = multi : one
    const int count = getLength();
    const char srcType = _types[0];
    const int srcUnitSize = MemoryUtils::getUnitSize(srcType);

    const char outType = MemoryUtils::computeType(srcType, MemoryUtils::computeType(val->getTypes()));
    const int outUnitSize = MemoryUtils::getUnitSize(outType);
    auto out_mem = SkMemory::create(outType, val->getRowCount());

    if(val->isSingleType()){
        SkMemory *m1;
        for (int i = 0, rc = val->getRowCount(); i < rc; ++i) {
            m1 = val->array[i];
            double value = MemoryUtils::pile(data, srcType, m1->data, m1->_types[0], count);
            MemoryUtils::setValue(out_mem->data, outType, i * outUnitSize, value);
        }
    } else{
        SkAnyMemory *m1;
        double total;
        size_t val_bytesIndex;
        size_t val_type_len;

        for (int i = 0, rc = val->getRowCount(); i < rc; ++i) {
            m1 = val->anyArray[i];
            total = 0;
            const char* types = m1->_types;
            val_type_len = strlen(types);
            val_bytesIndex = 0;
            for (int j = 0; j < count; ++j) {
                char type = types[j % val_type_len];
                total += MemoryUtils::multiple(m1->data, type, val_bytesIndex, data, srcType, j * srcUnitSize);
                val_bytesIndex += MemoryUtils::getUnitSize(type);
            }
            MemoryUtils::setValue(out_mem->data, outType, i * outUnitSize, total);
        }
    }
    return out_mem;
}
SkMemory* SkMemory::create(const char *type, int count) {
   auto pMemory = new SkMemory();
   pMemory->_types = type;
   pMemory->size = MemoryUtils::getUnitSize(type[0]) * count;
   pMemory->data = malloc(pMemory->size);
   return pMemory;
}
SkMemory* SkMemory::create(char type, int count) {
    char* val = static_cast<char *>(malloc(2));
    val[0] = type;
    val[1] = '\0';
    SkMemory *pMemory = create(val, count);
    pMemory->_needFreePtr = 1;
    return pMemory;
}

SkMemory *SkMemory::extract(size_t start, size_t end1) {
    if(end1 > getLength() || end1 <= start){
        return nullptr;
    }
    SkMemory *pMemory = SkMemory::create(_types, end1 - start);
    const int unitSize = MemoryUtils::getUnitSize(_types[0]);
    unsigned char* da = static_cast<unsigned char *>(data);
    da += start * unitSize;
    memcpy(pMemory->data, (void*)da, unitSize * (end1 - start));
    return pMemory;
}

SkMemory *SkMemory::kickOut(size_t index) {
    if(index >= getLength()){
        return nullptr;
    }
    SkMemory *pMemory = SkMemory::create(_types, getLength()-1);
    unsigned char* outAddr = static_cast<unsigned char *>(pMemory->data);

    const int unitSize = MemoryUtils::getUnitSize(_types[0]);
    unsigned char* da = static_cast<unsigned char *>(data);
    if(index > 0){
        memcpy(outAddr, (void*)da, unitSize * index);
        outAddr += unitSize * index;
    }
    da += unitSize * (index + 1);
    memcpy(outAddr, (void*)da, (pMemory->getLength() - index) * unitSize);
    return pMemory;
}
SkMemory* SkMemory::mergeUnit(char dstType, size_t unitCount,Func_Merge Merge,void *ctx, const char** errorMsg) {
    *errorMsg = NULL;
    if(getLength() % unitCount != 0) return NULL; // can't merge unit
    size_t dstLen = getLength() / unitCount;
    auto pMemory = SkMemory::create(dstType, dstLen);

    const char srcType = _types[0];
    const int srcUnitSize = MemoryUtils::getUnitSize(srcType);

    double eles[unitCount];
    double val;
    for (int i = 0; i < dstLen; ++i) {
        for (int j = 0; j < unitCount; ++j) {
            eles[j] = MemoryUtils::getValue(data, srcType, (i * unitCount + j) * srcUnitSize);
        }
        val = Merge(ctx, errorMsg, eles, unitCount);
        if(*errorMsg != NULL){
            goto fail;
        }
        pMemory->setValue(i, val);
    }
    return pMemory;
fail:
    pMemory->unRefAndDestroy();
    return NULL;
}
SkMemory* SkMemory::splitUnit(char dstType, size_t unitCount, Func_Split Split,void *ctx, const char** errorMsg) {
    *errorMsg = NULL;
    auto srcLen = getLength();
    size_t dstLen = srcLen * unitCount;
    auto pMemory = SkMemory::create(dstType, dstLen);
    
    const char srcType = _types[0];
    const int srcUnitSize = MemoryUtils::getUnitSize(srcType);

    double val;
    double eles[unitCount];
    for (int i = 0; i < srcLen; ++i) {
        val = MemoryUtils::getValue(data, srcType, i * srcUnitSize);
        Split(ctx, errorMsg, val, unitCount, eles);
        if(*errorMsg != NULL){
            goto fail;
        }
        for (size_t j = 0; j < unitCount; ++j) {
            pMemory->setValue(i * unitCount + j, eles[j]);
        }
    }
    return pMemory;
fail:
    pMemory->unRefAndDestroy();
    return NULL;
}

bool SkMemory::equals(SkMemory *o) {
    if(o == nullptr || getTypes()[0] != o->getTypes()[0]){
        return false;
    }
    if(getLength() != o->getLength()){
        return false;
    }
    const int c = getLength();
    const char type = _types[0];
    const int unitSize = MemoryUtils::getUnitSize(type);
    double v1, v2;
    for (int i = 0; i < c; ++i) {
        v1 = MemoryUtils::getValue(data, type, unitSize * i);
        v2 = MemoryUtils::getValue(o->data, type, unitSize * i);
        //compare double or float need fabs/fabsf
        if(!MathUtils::doubleEqual(v1, v2)){
            return false;
        }
    }
    return true;
}

SkMemory* SkMemory::reshape(int count, char t, double defVal) {
    const auto srcType = getTypes()[0];
    const auto srcUnitSize = MemoryUtils::getUnitSize(srcType);
    if(t == DEF_RESHAPE_TYPE){
        t = srcType;
    }

    auto pMemory = SkMemory::create(t, count);
    const auto unitSize = MemoryUtils::getUnitSize(t);
    const auto curSize = getLength();
    double val;
    for (int i = 0; i < count; ++i) {
        if(i >= curSize){
            val = defVal;
        } else{
            val = MemoryUtils::getValue(data, srcType, srcUnitSize * i);
        }
        MemoryUtils::setValue(pMemory->data, t, i * unitSize, val);
    }
    return pMemory;
}
SkMemory* SkMemory::reshapeBefore(int count, char t, double defVal) {
    const auto srcType = getTypes()[0];
    const auto srcUnitSize = MemoryUtils::getUnitSize(srcType);
    if(t == DEF_RESHAPE_TYPE){
        t = srcType;
    }

    auto outMem = SkMemory::create(t, count);
    const auto outUnitSize = MemoryUtils::getUnitSize(t);
    const auto curSize = getLength();

    double val;
    const int beforeCount = count - curSize > 0 ? count - curSize : 0;
    for (int i = 0; i < count; ++i) {
        if(i < beforeCount || i >= beforeCount + curSize){
            val = defVal;
        } else{
            val = MemoryUtils::getValue(data, srcType, srcUnitSize * (i - beforeCount));
        }
        MemoryUtils::setValue(outMem->data, t, i * outUnitSize, val);
    }
    return outMem;
}

SkMemory *SkMemory::concat(SkMemory *oth, int resultCount, char resultType, double defVal) {

    const auto srcType = getTypes()[0];
    const auto srcUnitSize = MemoryUtils::getUnitSize(srcType);
    auto srcSize = getLength();
    if(resultType == DEF_RESHAPE_TYPE){
        resultType = MemoryUtils::computeType(srcType, oth->getTypes()[0]);
    }

    auto outMem = SkMemory::create(resultType, resultCount);
    const auto out_unitSize = MemoryUtils::getUnitSize(resultType);

    const auto othType = oth->getTypes()[0];
    const auto othUnitSize = MemoryUtils::getUnitSize(othType);
    auto othSize = oth->getLength();
    double val;
    for (int i = 0; i < resultCount; ++i) {
        if(i < srcSize){
            val = MemoryUtils::getValue(data, srcType, i * srcUnitSize);
        } else if(i < srcSize + othSize){
            val = MemoryUtils::getValue(oth->data, othType, (i - srcSize) * othUnitSize);
        } else{
            val = defVal;
        }
        MemoryUtils::setValue(outMem->data, resultType, out_unitSize * i, val);
    }
    return outMem;
}
SkMemory *SkMemory::concat(SkAnyMemory *oth, int resultCount, char resultType, double defVal) {
    //src info
    const auto srcType = getTypes()[0];
    const auto srcUnitSize = MemoryUtils::getUnitSize(srcType);
    auto srcSize = getLength();
    //handle default type
    if(resultType == DEF_RESHAPE_TYPE){
        resultType = MemoryUtils::computeType(srcType, MemoryUtils::computeType(oth->getTypes()));
    }
    //out info
    auto outMem = SkMemory::create(resultType, resultCount);
    const auto out_unitSize = MemoryUtils::getUnitSize(resultType);
    //other info
    const auto len_othTypes = strlen(oth->getTypes());
    auto othSize = oth->getLength();
    char othType;
    size_t othBytes = 0;

    double val;
    for (int i = 0; i < resultCount; ++i) {
        if(i < srcSize){
            val = MemoryUtils::getValue(data, srcType, i * srcUnitSize);
        } else if(i < srcSize + othSize){
            othType = oth->getTypes()[(i- srcSize) % len_othTypes];
            val = MemoryUtils::getValue(oth->data, othType, othBytes);
            othBytes += MemoryUtils::getUnitSize(othType);
        } else{
            val = defVal;
        }
        MemoryUtils::setValue(outMem->data, resultType, out_unitSize * i, val);
    }
    return outMem;
}

SkMemory *SkMemory::flip(bool copy) {
    auto size = getLength();
    if(copy){
        auto rt = getTypes()[0];
        auto mem = SkMemory::create(getTypes(), size);
        auto unitSize = MemoryUtils::getUnitSize(rt);
        //i , size - i - 1
        for (int i = 0; i < size; ++i) {
            MemoryUtils::setValue(mem->data, rt, unitSize * i,
                                  MemoryUtils::getValue(data, rt, unitSize * (size - i - 1)));
        }
        return mem;
    } else{
        for (int i = 0,c = getLength()/ 2; i < c; ++i) {
            this->swapValue(i, size - i - 1);
        }
    }
    return this;
}
//将当前的memory 当作斜线的元素数组 组成mat.
SkMemoryMatrix* SkMemory::diag(int k, char t,double defVal) {
    auto curC = getLength();
    int n = getLength() + abs(k);
    if(t == DEF_RESHAPE_TYPE){
        t = getTypes()[0];
    }

    //init mat
    auto mat = new SkMemoryMatrix(n, true);
    for (int i = 0; i < n; ++i) {
        mat->array[i] = SkMemory::create(t, n);
    }

    //要改变的数据Index
    //从左下角。n行0列开始，
    //colIndex, rowIndex
    int lt[2];
    int rb[2];
    if(k > 0){
        lt[0] = k;
        lt[1] = 0;
        rb[0] = n - 1;
        rb[1] = n - 1 - k;
    } else{
        lt[0] = 0;
        lt[1] = -k;
        rb[0] = n - 1 + k;
        rb[1] = n - 1;
    }
    //start set element value
    HMath::LinearEquation line(lt[0], lt[1], rb[0], rb[1]);
    double result;
    double ele;
    int idx;
    for (int i = 0; i < n; ++i) { //col
        idx= k <= 0 ? i : i - 1;
        if(idx < curC){
            getValue(idx, &ele);
        }
        for (int j = 0; j < n; ++j) { //row
            result = !line.isIn(i, j) ? defVal : ele;
            mat->setValue(j, i, result);
        }
    }
    return mat;
}

void SkMemory::swapValue(int index1, int index2) {
    auto rt = getTypes()[0];
    auto unitSize = MemoryUtils::getUnitSize(getTypes()[0]);
    double val1 = MemoryUtils::getValue(data, rt, unitSize * index1);
    MemoryUtils::setValue(data, rt, unitSize * index1,
                          MemoryUtils::getValue(data, rt, unitSize * index2));
    MemoryUtils::setValue(data, rt, unitSize * index2, val1);
}

bool SkMemory::setValue(int index, double val) {
    if(index < 0 || index >= getLength()){
        return false;
    }
    auto unitSize = MemoryUtils::getUnitSize(getTypes()[0]);
    MemoryUtils::setValue(data, getTypes()[0], unitSize * index, val);
    return true;
}
bool SkMemory::getValue(int index, double* result) {
    if(index < 0 || index >= getLength()){
        return false;
    }
    auto unitSize = MemoryUtils::getUnitSize(getTypes()[0]);
    *result = MemoryUtils::getValue(data, getTypes()[0], unitSize * index);
    return true;
}

//======================= SKAnyMemory ===================================
SkAnyMemory::SkAnyMemory(lua_State *L, const char *types): SkAnyMemory(L, types, -1){

}
SkAnyMemory::SkAnyMemory(lua_State *L, const char *types, int tableIndex) : SimpleMemory(){
    //if assigned table index. means only one table
    _tabCount = tableIndex >= 1 ? 1 : lua_gettop(L);
    _types = types;

    size_t len = strlen(types);
    //
    const size_t tabLen = lua_rawlen(L, tableIndex >=1 ? tableIndex : 1); //all size must be the same
    if(tabLen % len != 0){
        luaL_error(L, "wrong length of lua table");
        return ;
    }
    _elementCount = tabLen;
    //compute size(bytes) of one table.
    int bytes = 0;
    for (int i = 0; i < tabLen; ++i) {
        bytes += MemoryUtils::getUnitSize(types[i % len]);
    }
    this->size = bytes * _tabCount;
    data = malloc(this->size);

    //init data
    int totalIndex = 0;
    if(tableIndex >= 1){
        for (int idx = 0; ; ++idx) {
            int type = lua_rawgeti(L, tableIndex, idx + 1);
            if(type == LUA_TNIL){
                lua_pop(L, 1);
                break;
            }
            char t = types[idx % len];
            if(MemoryUtils::read(L, t, data, totalIndex) < 0){
                lua_pop(L, 1);
                goto fail;
            }
            lua_pop(L, 1);
            totalIndex += MemoryUtils::getUnitSize(t);
        }
    } else{
        for (int i = 0; i < _tabCount; ++i) {
            size_t rawlen = lua_rawlen(L, i + 1);
            if(rawlen != tabLen){
                luaL_error(L, "table array length not the same");
                goto fail;
            }
            for (int idx = 0; ; ++idx) {
                int type = lua_rawgeti(L, i + 1, idx + 1);
                if(type == LUA_TNIL){
                    lua_pop(L, 1);
                    break;
                }
                char t = types[idx % len];
                if(MemoryUtils::read(L, t, data, totalIndex) < 0){
                    lua_pop(L, 1);
                    goto fail;
                }
                lua_pop(L, 1);
                totalIndex += MemoryUtils::getUnitSize(t);
            }
        }
    }
    return ;
    fail:
      unRef();
      FREE_POINTER(data)
      size = 0;
      _elementCount = 0;
      _tabCount = 0;
}
SkAnyMemory::SkAnyMemory(const char *types, int count): SkAnyMemory(types, count, true){

}
SkAnyMemory::SkAnyMemory(const char *types, int count, bool init): SimpleMemory(), _types(types), _tabCount(count){
    size_t len = strlen(types);
    _elementCount = len;

    int size = 0;
    for (int i = 0; i < len; ++i) {
        size += MemoryUtils::getUnitSize(types[i]);
    }
    SkASSERT(size > 0);
    this->size = size * count;
    data = malloc(this->size);
    //init array
    if(init){
        size_t bytes = 0;
        char t;
        for (int i = 0; i < _tabCount; ++i) {
            for (int j = 0; j < _elementCount; ++j) {
                t = _types[j % len];
                MemoryUtils::init(t, data, bytes);
                bytes += MemoryUtils::getUnitSize(t);
            }
        }
    }
}
void SkAnyMemory::toString(SB::StringBuilder &sb) {
    size_t len = strlen(_types);
    size_t bytes = 0;
    char t;
    sb << "{";
    for (int i = 0; i < _tabCount; ++i) {
        for (int j = 0; j < _elementCount; ++j) {
            t = _types[j % len];
            if(i != 0 || j != 0){
                sb << ",";
            }
            MemoryUtils::toString(sb, t, data, bytes);
            bytes += MemoryUtils::getUnitSize(t);
        }
    }
    sb << "}";
}
SkAnyMemory* SkAnyMemory::copy() {
    SkAnyMemory *pMemory = new SkAnyMemory(_types, _tabCount, false);
    memcpy(pMemory->data, data, size);
    return pMemory;
}

int SkAnyMemory::convert(lua_State* L, const char *ts) {
    const int dstStrLen = strlen(ts);
    if(dstStrLen == 1){
        const int dstUnitSize = MemoryUtils::getUnitSize(ts[0]);
        auto pMemory = SkMemory::create(ts, getLength());
        //convert data

        size_t len = strlen(_types);
        char t;
        size_t srcBytes = 0;
        int dstIndex = 0;
        for (int i = 0; i < _tabCount; ++i) {
            for (int j = 0; j < _elementCount; ++j) {
                t = _types[j % len];
                MemoryUtils::convert(data, t, srcBytes, pMemory->data, ts[0], dstIndex * dstUnitSize);
                srcBytes += MemoryUtils::getUnitSize(t);
                dstIndex ++;
            }
        }
        LuaUtils::push_ptr(L, pMemory);
        return 1;
    } else{
        //error mismatch
        if(getLength() % dstStrLen != 0){
            return luaL_error(L, "length mismatch");
        }
        size_t tabCount = getLength() / dstStrLen;
        const auto pMemory = new SkAnyMemory(ts, tabCount, false);
        //copy data
        const size_t srcStrLen = strlen(_types);
        char srcType, dstType;
        size_t  dstBytes = 0;
        size_t  srcBytes = 0;
        for (int i = 0, length = getLength(); i < length; ++i) {
            dstType = ts[i % dstStrLen];
            srcType = _types[i % srcStrLen];
            MemoryUtils::convert(data, srcType, srcBytes, pMemory->data, dstType, dstBytes);
            srcBytes += MemoryUtils::getUnitSize(srcType);
            dstBytes += MemoryUtils::getUnitSize(dstType);
        }
        LuaUtils::push_ptr(L, pMemory);
        return 1;
    }
}
int SkAnyMemory::foreach(lua_State *L) {
    const size_t srcStrLen = strlen(_types);
    char srcType;
    size_t  srcBytes = 0;
    for (int i = 0, length = getLength(); i < length; ++i) {
        srcType = _types[i % srcStrLen];
        lua_pushvalue(L, -1); //push func
        lua_pushinteger(L, i); // push index
        MemoryUtils::write(L, srcType, data, srcBytes);
        if(lua_pcall(L, 2, 0, 0) != LUA_OK){
            return luaL_error(L, "call SkAnyMemory.foreach(...) failed.");
        }
        srcBytes += MemoryUtils::getUnitSize(srcType);
    }
    return 0;
}
SkAnyMemory* SkAnyMemory::_mul(double val) {
    SkAnyMemory *pMemory = copy();
    const bool isInt = floor(val) == val;
    const size_t srcStrLen = strlen(_types);
    char srcType;
    size_t  srcBytes = 0;
    for (int i = 0, length = getLength(); i < length; ++i) {
        srcType = _types[i % srcStrLen];
        if(isInt){
            MemoryUtils::multiple(pMemory->data, srcType, srcBytes, (lua_Integer)val);
        } else{
            MemoryUtils::multiple(pMemory->data, srcType, srcBytes, val);
        }
        srcBytes += MemoryUtils::getUnitSize(srcType);
    }
    return pMemory;
}
SkAnyMemory* SkAnyMemory::_mul(SkMemory *val) {
    if(getLength() != val->getLength()){
        return nullptr;
    }
    SkAnyMemory* out = new SkAnyMemory(_types, _tabCount, false);
    const char val_type = val->_types[0];
    const int val_unit_size = MemoryUtils::getUnitSize(val_type);

    const auto len_type = strlen(_types);
    char srcType;
    size_t srcBytes = 0;
    for (int j = 0; j < getLength(); ++j) {
        srcType = _types[j % len_type];
        MemoryUtils::multiple(data, srcType, srcBytes,
                              val->data, val_type, j * val_unit_size,
                              out->data, srcType, srcBytes);
        srcBytes += MemoryUtils::getUnitSize(srcType);
    }
    return out;
}
SkAnyMemory* SkAnyMemory::_mul(SkAnyMemory *val) {
    if(getLength() != val->getLength()){
        return nullptr;
    }
    SkAnyMemory* out = new SkAnyMemory(_types, _tabCount, false);
    const auto cur_type_len = strlen(_types);
    char curType;
    size_t curBytes = 0;

    const auto val_type_len = strlen(val->_types);
    char valType;
    size_t valBytes = 0;
    for (int j = 0; j < getLength(); ++j) {
        curType = _types[j % cur_type_len];
        valType = val->_types[j % val_type_len];

        MemoryUtils::multiple(data, curType, curBytes,
                              val->data, valType, valBytes,
                              out->data, curType, curBytes);

        curBytes += MemoryUtils::getUnitSize(curType);
        valBytes += MemoryUtils::getUnitSize(valType);
    }
    return out;
}
SkMemory* SkAnyMemory::_mul(SkMemoryMatrix *val, bool dotMat) {
    if(dotMat){
        return dot(val);
    }
    if(getLength() != val->getLength()){
        return nullptr;
    }
    const int columnCount = val->getColumnCount();
    const int srcCount = getLength();
    const size_t srcType_len = strlen(getTypes());

    const char outType = MemoryUtils::computeType(MemoryUtils::computeType(_types),
            MemoryUtils::computeType(val->getTypes()));
    const int outUnitSize = MemoryUtils::getUnitSize(outType);
    auto out_mem = SkMemory::create(outType, srcCount);

    double value;
    double srcVal;
    if(val->isSingleType()){
        char srcType;
        size_t srcBytesIndex = 0;

        const char dstType = val->getTypes()[0];
        const int dstUnitSize = MemoryUtils::getUnitSize(dstType);
        SkMemory *mem_dst;
        for (int i = 0; i < srcCount; ++i) {
            srcType = getTypes()[i % srcType_len];
            mem_dst = val->array[i];
            value = 0;
            for (int j = 0; j < columnCount; ++j) {
                value += MemoryUtils::getValue(mem_dst->data, dstType, j* dstUnitSize);
            }
            srcVal = MemoryUtils::getValue(data, srcType, srcBytesIndex);
            srcBytesIndex += MemoryUtils::getUnitSize(srcType);

            MemoryUtils::setValue(out_mem->data, outType, i * outUnitSize, srcVal * value);
        }
    } else{
        char srcType;
        size_t srcBytesIndex = 0;


        SkAnyMemory *mem_dst;
        char dstType;
        size_t dstType_len;
        size_t dstBytesIndex;

        for (int i = 0; i < srcCount; ++i) {
            srcType = getTypes()[i % srcType_len];

            mem_dst = val->anyArray[i];
            value = 0;
            dstType_len = strlen(mem_dst->getTypes());
            dstBytesIndex = 0;
            for (int j = 0; j < columnCount; ++j) {
                dstType = mem_dst->getTypes()[j % dstType_len];
                value += MemoryUtils::getValue(mem_dst->data, dstType, dstBytesIndex);
                dstBytesIndex += MemoryUtils::getUnitSize(dstType);
            }
            srcVal = MemoryUtils::getValue(data, srcType, srcBytesIndex);
            srcBytesIndex += MemoryUtils::getUnitSize(srcType);

            MemoryUtils::setValue(out_mem->data, outType, i * outUnitSize, srcVal * value);
        }
    }
    return out_mem;
}
SkMemory* SkAnyMemory::dot(SkMemoryMatrix *val) {
    if(getLength() != val->getColumnCount()){
        return nullptr;//un support
    }
    //src info
    const int srcLen = getLength();
    size_t src_type_len = strlen(_types);
    size_t srcBytes;
    char srcType;

    //choose a best type for dot
    const char outType = MemoryUtils::computeType(MemoryUtils::computeType(_types),
            MemoryUtils::computeType(val->getTypes()));
    const int outUnitSize = MemoryUtils::getUnitSize(outType);
    auto pMemory = SkMemory::create(outType, val->getRowCount());

    double value;
    double srcVal;
    double dstVal;

    if(val->isSingleType()){
        //dst info
        SkMemory *dstMem;
        const char dstType = val->array[0]->_types[0];
        const size_t dstUnitSize = MemoryUtils::getUnitSize(dstType);

        for (int i = 0, rc = val->getRowCount(); i < rc; ++i) {
            dstMem = val->array[i];
            value = 0;
            srcBytes = 0;
            //make a skm multiple a row of mat
            for (int j = 0; j < srcLen; ++j) {
                srcType = _types[j % src_type_len];
                srcVal = MemoryUtils::getValue(data, srcType, srcBytes);
                dstVal = MemoryUtils::getValue(dstMem->data, dstType, j * dstUnitSize);
                value += srcVal * dstVal;
                srcBytes += MemoryUtils::getUnitSize(srcType);
            }
            MemoryUtils::setValue(pMemory->data, outType, i * outUnitSize, value);
        }
    } else{
        SkAnyMemory *dstMem;
        const char* dstTypes = val->anyArray[0]->_types;
        size_t dstType_Len = strlen(dstTypes);
        char dstType;
        size_t dstBytes;

        for (int i = 0, rc = val->getRowCount(); i < rc; ++i) {
            dstMem = val->anyArray[i];
            value = 0;
            srcBytes = 0;
            dstBytes = 0;
            //make a skm multiple a row of mat
            for (int j = 0; j < srcLen; ++j) {
                srcType = _types[j % src_type_len];
                dstType = dstTypes[j % dstType_Len];

                srcVal = MemoryUtils::getValue(data, srcType, srcBytes);
                dstVal = MemoryUtils::getValue(dstMem->data, dstType, dstBytes);
                value += srcVal * dstVal;
                srcBytes += MemoryUtils::getUnitSize(srcType);
                dstBytes += MemoryUtils::getUnitSize(dstType);
            }
            MemoryUtils::setValue(pMemory->data, outType, i * outUnitSize, value);
        }
    }
    return pMemory;
}
bool SkAnyMemory::equals(SkAnyMemory *o) {
    if(o == NULL || strcmp(getTypes(), o->getTypes()) != 0){
        return false;
    }
    if(getLength() != o->getLength()){
        return false;
    }
    const size_t len_type = strlen(getTypes());
    char type;
    size_t bytes = 0;
    for (int i = 0, c = getLength(); i < c; ++i) {
        type = getTypes()[i % len_type];
        if(!MathUtils::doubleEqual(MemoryUtils::getValue(data, type, bytes),
                                   MemoryUtils::getValue(o->data, type, bytes))){
            return false;
        }
        bytes += MemoryUtils::getUnitSize(type);
    }
    return true;
}
bool SkAnyMemory::setValue(int index, double val) {
    if(index < 0 || index >= getLength()){
        return false;
    }
    auto types = getTypes();
    MemoryUtils::setValue(data, types[index % strlen(types)],
                          MemoryUtils::computeBytesIndex(types, index), val);
    return true;
}
bool SkAnyMemory::getValue(int index, double *result) {
    if(index < 0 || index >= getLength()){
        return false;
    }
    auto types = getTypes();
    *result = MemoryUtils::getValue(data, types[index % strlen(types)], MemoryUtils::computeBytesIndex(types, index));
    return true;
}

SkMemory *SkAnyMemory::extract(size_t start, size_t end1) {
    if(end1 > getLength() || end1 <= start){
        return nullptr;
    }
    SkMemory *outMem = SkMemory::create(MemoryUtils::computeType(_types), end1 - start);
    const char outType = outMem->getTypes()[0];
    const int out_unitSize = MemoryUtils::getUnitSize(outType);

    size_t srcBytes = MemoryUtils::computeBytesIndex(_types, start);
    const size_t len_srcType = strlen(_types);
    char srcType;
    double value;
    for (size_t i = start; i < end1; ++i) {
        srcType = _types[i % len_srcType];
        value = MemoryUtils::getValue(data, srcType, srcBytes);
        MemoryUtils::setValue(outMem->data, outType, out_unitSize * (i - start), value);
        srcBytes += MemoryUtils::getUnitSize(srcType);
    }
    return outMem;
}

SkMemory *SkAnyMemory::kickOut(size_t index) {
    if(index >= getLength()){
        return nullptr;
    }
    SkMemory *outMem = SkMemory::create(MemoryUtils::computeType(_types), getLength() - 1);
    const char outType = outMem->getTypes()[0];
    const int out_unitSize = MemoryUtils::getUnitSize(outType);

    const size_t len_srcType = strlen(_types);
    char srcType;
    double value;
    size_t srcBytes = 0;
    for (int i = 0, c = getLength(); i < c; ++i) {
        srcType = _types[i % len_srcType];
        //ignore the index value
        if(i == index){
            srcBytes += MemoryUtils::getUnitSize(srcType);
            continue;
        }
        value = MemoryUtils::getValue(data, srcType, srcBytes);
        if(i < index){
            MemoryUtils::setValue(outMem->data, outType, out_unitSize * i, value);
        } else{
            MemoryUtils::setValue(outMem->data, outType, out_unitSize * (i - 1), value);
        }
        srcBytes += MemoryUtils::getUnitSize(srcType);
    }

    return outMem;
}

SkMemory* SkAnyMemory::mergeUnit(char dstType, size_t unitCount,Func_Merge Merge, void *ctx, const char** error) {
    *error = NULL;
    if(getLength() % unitCount != 0) return NULL; // can't merge unit
    size_t dstLen = getLength() / unitCount;
    auto pMemory = SkMemory::create(dstType, dstLen);

    const char len_srcType = strlen(_types);
    char srcType;
    size_t srcBytesIndex = 0;

    double eles[unitCount];
    double val;
    for (int i = 0; i < dstLen; ++i) {
        for (int j = 0; j < unitCount; ++j) {
            srcType = _types[(i * unitCount + j) % len_srcType];
            eles[j] = MemoryUtils::getValue(data, srcType, srcBytesIndex);
            srcBytesIndex += MemoryUtils::getUnitSize(srcType);
        }
        val = Merge(ctx, error, eles, unitCount);
        if(*error != NULL){
            goto fail;
        }
        pMemory->setValue(i, val);
    }
    return pMemory;
fail:
    pMemory->unRefAndDestroy();
    return NULL;
}
SkMemory* SkAnyMemory::splitUnit(char dstType, size_t unitCount, Func_Split Split, void *ctx, const char** error) {
    *error = NULL;
    auto srcLen = getLength();
    size_t dstLen = srcLen * unitCount;
    auto pMemory = SkMemory::create(dstType, dstLen);

    const char len_srcType = strlen(_types);
    char srcType;
    size_t srcBytesIndex = 0;

    double val;
    double eles[unitCount];
    for (int i = 0; i < srcLen; ++i) {
        srcType = _types[i % len_srcType];
        val = MemoryUtils::getValue(data, srcType, srcBytesIndex);
        srcBytesIndex += MemoryUtils::getUnitSize(srcType);

        Split(ctx, error, val, unitCount, eles);
        if(*error != NULL){
            goto fail;
        }
        for (size_t j = 0; j < unitCount; ++j) {
            pMemory->setValue(i * unitCount + j, eles[j]);
        }
    }
    return pMemory;
fail:
    pMemory->unRefAndDestroy();
    return NULL;
}

SkMemory *SkAnyMemory::reshape(int count, char t, double defVal) {
    const auto srcTypes = getTypes();
    auto len_srcTypes = strlen(srcTypes);
    const auto srcLen = getLength();

    if(t == DEF_RESHAPE_TYPE){
        t = MemoryUtils::computeType(srcTypes);
    }

    auto outMem = SkMemory::create(t, count);
    const auto out_unitSize = MemoryUtils::getUnitSize(t);
    double val;
    char srcType;
    size_t srcBytes = 0;
    for (int i = 0; i < count; ++i) {
        if(i >= srcLen){
            MemoryUtils::setValue(outMem->data, t, i * out_unitSize, defVal);
        } else{
            srcType = srcTypes[i % len_srcTypes];
            val = MemoryUtils::getValue(data, srcType, srcBytes);
            MemoryUtils::setValue(outMem->data, t, i * out_unitSize, val);
            srcBytes += MemoryUtils::getUnitSize(srcType);
        }
    }
    return outMem;
}
SkMemory* SkAnyMemory::reshapeBefore(int count, char t, double defVal) {
    const size_t len_srcTypes = strlen(getTypes());
    char srcType;
    size_t srcBytes = 0;
    if(t == DEF_RESHAPE_TYPE){
        t = MemoryUtils::computeType(getTypes());
    }

    auto outMem = SkMemory::create(t, count);
    const auto outUnitSize = MemoryUtils::getUnitSize(t);
    const auto curSize = getLength();

    double val;
    const int beforeCount = count - curSize > 0 ? count - curSize : 0;
    for (int i = 0; i < count; ++i) {
        if(i < beforeCount || i >= beforeCount + curSize){
            val = defVal;
        } else{
            srcType = getTypes()[(i - beforeCount) % len_srcTypes];
            val = MemoryUtils::getValue(data, srcType, srcBytes);
            srcBytes += MemoryUtils::getUnitSize(srcType);
        }
        MemoryUtils::setValue(outMem->data, t, i * outUnitSize, val);
    }
    return outMem;
}

SkMemory *SkAnyMemory::concat(SkMemory *oth, int resultCount, char resultType, double defVal) {
    //src info
    const auto len_srcTypes = strlen(getTypes());
    auto srcSize = getLength();
    char srcType;
    size_t srcBytes = 0;
    if(resultType == DEF_RESHAPE_TYPE){
        resultType = MemoryUtils::computeType(oth->getTypes()[0], MemoryUtils::computeType(getTypes()));
    }
    //out info
    auto outMem = SkMemory::create(resultType, resultCount);
    const auto out_unitSize = MemoryUtils::getUnitSize(resultType);
    //other info
    auto othSize = oth->getLength();
    char othType = oth->getTypes()[0];
    auto othUnitSize = MemoryUtils::getUnitSize(othType);

    double val;
    for (int i = 0; i < resultCount; ++i) {
        if(i < srcSize){
            srcType = getTypes()[i % len_srcTypes];
            val = MemoryUtils::getValue(data, srcType, srcBytes);
            srcBytes += MemoryUtils::getUnitSize(srcType);
        } else if(i < srcSize + othSize){
            val = MemoryUtils::getValue(oth->data, othType, (i - srcSize) * othUnitSize);
        } else{
            val = defVal;
        }
        MemoryUtils::setValue(outMem->data, resultType, out_unitSize * i, val);
    }
    return outMem;
}
SkMemory* SkAnyMemory::concat(SkAnyMemory *oth, int resultCount, char resultType, double defVal) {
    //src info
    const auto len_srcTypes = strlen(getTypes());
    auto srcSize = getLength();
    char srcType;
    size_t srcBytes = 0;
    if(resultType == DEF_RESHAPE_TYPE){
        resultType = MemoryUtils::computeType(MemoryUtils::computeType(getTypes()),
                MemoryUtils::computeType(oth->getTypes()));
    }
    //out info
    auto outMem = SkMemory::create(resultType, resultCount);
    const auto out_unitSize = MemoryUtils::getUnitSize(resultType);
    //other info
    const auto len_othTypes = strlen(oth->getTypes());
    auto othSize = oth->getLength();
    char othType;
    size_t othBytes = 0;

    double val;
    for (int i = 0; i < resultCount; ++i) {
        if(i < srcSize){
            srcType = getTypes()[i % len_srcTypes];
            val = MemoryUtils::getValue(data, srcType, srcBytes);
            srcBytes += MemoryUtils::getUnitSize(srcType);
        } else if(i < srcSize + othSize){
            othType = oth->getTypes()[(i- srcSize) % len_othTypes];
            val = MemoryUtils::getValue(oth->data, othType, othBytes);
            othBytes += MemoryUtils::getUnitSize(othType);
        } else{
            val = defVal;
        }
        MemoryUtils::setValue(outMem->data, resultType, out_unitSize * i, val);
    }
    return outMem;
}

SkMemory *SkAnyMemory::flip() {
    const auto size = getLength();
    auto srcTypes = getTypes();
    const auto src_lenTypes = strlen(srcTypes);

    auto rt = MemoryUtils::computeType(srcTypes);
    auto unitSize = MemoryUtils::getUnitSize(rt);
    auto pMemory = SkMemory::create(rt, size);
    
    char type;
    double val;
    for (int i = 0; i < size; ++i) {
        type = srcTypes[(size - i - 1) % src_lenTypes];
        val = MemoryUtils::getValue(data, type, 
                MemoryUtils::computeBytesIndex(srcTypes, size - i - 1));
        MemoryUtils::setValue(pMemory->data, rt, i * unitSize, val);
    }
    return pMemory;
}
//将当前的memory 当作斜线的元素数组 组成mat.
SkMemoryMatrix* SkAnyMemory::diag(int k, char rt, double defVal) {
    auto curC = getLength();
    int n = getLength() + abs(k);
    if(rt == DEF_RESHAPE_TYPE){
        rt = MemoryUtils::computeType(getTypes());
    }

    //init mat. n *n
    auto mat = new SkMemoryMatrix(n, true);
    for (int i = 0; i < n; ++i) {
        mat->array[i] = SkMemory::create(rt, n);
    }
    //row-column
    //--- (0,1), (0,2), (0,3)...(0,n)
    //----(1,1), (1,2), (1,3)...(1,n)
    //----...
    //从左下角。n行0列开始，
    //colIndex, rowIndex
    int lt[2];
    int rb[2];
    if(k > 0){
        lt[0] = k;
        lt[1] = 0;
        rb[0] = n - 1;
        rb[1] = n - 1 - k;
    } else{
        lt[0] = 0;
        lt[1] = -k;
        rb[0] = n - 1 + k;
        rb[1] = n - 1;
    }
    //start set element value
    HMath::LinearEquation line(lt[0], lt[1], rb[0], rb[1]);
    double result;
    double ele;
    int idx;
    for (int i = 0; i < n; ++i) { //col
        idx= k <= 0 ? i : i - 1;
        if(idx < curC){
            getValue(idx, &ele);
        }
        for (int j = 0; j < n; ++j) { //row
            result = !line.isIn(i, j) ? defVal : ele;
            mat->setValue(j, i, result);
        }
    }
    return mat;
}

//--------------------- Sk memory matrix --------------------
SkMemoryMatrix::~SkMemoryMatrix() {
    destroyData();
}
SkMemoryMatrix::SkMemoryMatrix():IMemory(){

}
SkMemoryMatrix::SkMemoryMatrix(lua_State *L, const char *type): IMemory(){
    int tableCount = lua_gettop(L);
    count = tableCount;
    if(strlen(type) == 1){
        array = new SkMemory*[tableCount];
        for (int i = 0; i < tableCount; ++i) {
            array[i] = new SkMemory(L, i, 1, type);
            //LOGD("array[%d]: \n %s", i, array[i]->toString());
        }
        anyArray = nullptr;
    } else{
        anyArray = new SkAnyMemory*[tableCount];
        for (int i = 0; i < tableCount; ++i) {
            anyArray[i] = new SkAnyMemory(L, type, i + 1);
            //LOGD("array[%d]: \n %s", i, array[i]->toString());
        }
        array = nullptr;
    }
}
SkMemoryMatrix::SkMemoryMatrix(const char *type, int rowCount, int columnCount): IMemory() {
    count = rowCount;
    if(strlen(type) == 1){
        array = new SkMemory*[rowCount];
        for (int i = 0; i < rowCount; ++i) {
            array[i] = new SkMemory(type, columnCount);
        }
        anyArray = nullptr;
    } else{
        anyArray = new SkAnyMemory*[rowCount];
        for (int i = 0; i < rowCount; ++i) {
            anyArray[i] = new SkAnyMemory(type, columnCount);
        }
        array = nullptr;
    }
}
SkMemoryMatrix::SkMemoryMatrix(int count):SkMemoryMatrix(count, true) {
}
SkMemoryMatrix::SkMemoryMatrix(int count, bool singleType): IMemory(), count(count) {
    if(singleType){
        array = new SkMemory*[count];
        anyArray = nullptr;
    } else{
        anyArray = new SkAnyMemory*[count];
        array = nullptr;
    }
}
void SkMemoryMatrix::destroyData() {
    DESTROY_MEM_POINTER_ARRAY(array);
    DESTROY_MEM_POINTER_ARRAY(anyArray);
    count = 0;
}
bool SkMemoryMatrix::isValid() {
    return array != nullptr || anyArray != nullptr;
}
bool SkMemoryMatrix::isSingleType() {
    return array != nullptr;
}

bool SkMemoryMatrix::equals(SkMemoryMatrix *o) {
    if(o == nullptr || getRowCount() != o->getRowCount() || getColumnCount() != o->getColumnCount()){
        return false;
    }
    if(isSingleType()){
        for (int i = 0; i < count; ++i) {
            if(!array[i]->equals(o->array[i])){
                return false;
            }
        }
    } else{
        for (int i = 0; i < count; ++i) {
            if(!anyArray[i]->equals(o->anyArray[i])){
                return false;
            }
        }
    }
    return true;
}
int SkMemoryMatrix::read(SkMemoryMatrix *mem, lua_State *L,  void (*Push)(lua_State*, SkMemory*)) {
    //table, index
    lua_Integer index = lua_tointeger(L, 2);
    if(index >= mem->count){
        return luaL_error(L, "index(%d) out of range(%d).", index, mem->count);
    }
    Push(L, mem->array[index]);
    return 1;
}
int SkMemoryMatrix::write(SkMemoryMatrix *mem, lua_State *L, SkMemory *(*Pull)(lua_State *, int)){
    //table, index, val
    lua_Integer index = lua_tointeger(L, 2);
    if(index >= mem->count){
        return luaL_error(L, "index(%d) out of range(%d).", index, mem->count);
    }
    mem->array[index] = Pull(L, 3);
    return 0;
}
int SkMemoryMatrix::read(SkMemoryMatrix *mem, lua_State *L,  void (*Push)(lua_State*, SkAnyMemory*)) {
    //table, index
    lua_Integer index = lua_tointeger(L, 2);
    if(index >= mem->count){
        return luaL_error(L, "index(%d) out of range(%d).", index, mem->count);
    }
    Push(L, mem->anyArray[index]);
    return 1;
}
int SkMemoryMatrix::write(SkMemoryMatrix *mem, lua_State *L, SkAnyMemory *(*Pull)(lua_State *, int)){
    //table, index, val
    lua_Integer index = lua_tointeger(L, 2);
    if(index >= mem->count){
        return luaL_error(L, "index(%d) out of range(%d).", index, mem->count);
    }
    mem->anyArray[index] = Pull(L, 3);
    return 0;
}

int SkMemoryMatrix::getColumnCount() {
    if(array){
        return array[0]->getLength();
    }
    if(anyArray){
        return anyArray[0]->getLength();
    }
    return 0;
}

void SkMemoryMatrix::toString(SB::StringBuilder &ss) {
#if defined(_WIN32)
    ss << "[\r\n";
#else
    ss << "[\n";
#endif
    for (int i = 0; i < count; ++i) {
        if(array){
            array[i]->toString(ss);
        } else if(anyArray){
            anyArray[i]->toString(ss);
        } else{
            LOGE("wrong state of SkMemoryMatrix. called by toString");
        }
        if (i != count - 1) {
            ss << ",";
#if defined(_WIN32)
            ss << "\r\n";
#else
            ss << "\n";
#endif
        }
    }
#if defined(_WIN32)
    ss << "\r\n]";
#else
    ss << "\n]";
#endif
}
int SkMemoryMatrix::foreach(lua_State* L) {
    //upvalues... func
    if(isSingleType()){
        SkMemory *pMemory;
        char srcType;
        int unitSize;
        size_t totalBytes = 0;
        for (int i = 0; i < getRowCount(); ++i) {
            pMemory = array[i];
            srcType = pMemory->_types[0];
            unitSize = MemoryUtils::getUnitSize(srcType);

            lua_pushinteger(L, i);
            for (int j = 0, length = pMemory->getLength(); j < length; ++j) {
                //func, rowIndex, columnIndex, type, total_bytes, val
                lua_pushvalue(L, -2);         // func
                lua_pushvalue(L, -2);         // row index
                lua_pushinteger(L, i);            // column index
                lua_pushinteger(L, srcType);      // type
                lua_pushinteger(L, totalBytes);   // total bytes
                MemoryUtils::write(L, srcType, pMemory->data, j * unitSize);
                //func, holder, val
                if(lua_pcall(L, 5, 1, 0) != LUA_OK){
                    lua_pushvalue(L, -1);
                    return 1;
                }
            }
            totalBytes += pMemory->getLength() * unitSize;
            lua_pop(L, 1);
        }
    } else{
        SkAnyMemory *pMemory;
        char srcType;
        int len_type;
        size_t totalBytes = 0;
        //cur bytes for internal
        size_t curBytes;
        for (int i = 0; i < getRowCount(); ++i) {
            pMemory = anyArray[i];
            len_type = strlen(pMemory->_types);
            curBytes = 0;

            lua_pushinteger(L, i);
            for (int j = 0, length = pMemory->getLength(); j < length; ++j) {
                srcType = pMemory->_types[j % len_type];

                //func, rowIndex, columnIndex, type, total_bytes, val
                lua_pushvalue(L, -2);          //func
                lua_pushvalue(L, -2);          // row index
                lua_pushinteger(L, i);             // column index
                lua_pushinteger(L, srcType);       // type
                lua_pushinteger(L, totalBytes);    // total bytes
                MemoryUtils::write(L, srcType, pMemory->data, curBytes);
                if(lua_pcall(L, 5, 0, 0) != LUA_OK){
                    lua_pushvalue(L, -1);
                    return 1;
                }
                totalBytes += MemoryUtils::getUnitSize(srcType);
                curBytes +=  MemoryUtils::getUnitSize(srcType);
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
    return 0;
}
int SkMemoryMatrix::tiled(lua_State *L) {
    const auto types = getTypes();
    if(isSingleType()){
        const auto rowC = getRowCount();
        auto mem = SkMemory::create(types, getRowCount() * getColumnCount());

        auto addr = static_cast<unsigned char *>(mem->data);
        for (int i = 0; i < rowC; ++i) {
            memcpy((void*)addr, array[i]->data, array[i]->size);
            addr += array[i]->size;
        }
        LuaUtils::push_ptr(L, mem);
        return 1;
    } else{
        auto tabCount = anyArray[0]->_tabCount;
        const auto rowC = getRowCount();
        auto mem = new SkAnyMemory(types, tabCount * rowC, false);

        auto addr = static_cast<unsigned char *>(mem->data);
        for (int i = 0; i < rowC; ++i) {
            memcpy((void*)addr, anyArray[i]->data, anyArray[i]->size);
            addr += anyArray[i]->size;
        }
        LuaUtils::push_ptr(L, mem);
        return 1;
    }
}

bool SkMemoryMatrix::getValue(int rowIndex, int colIndex, double* result) {
    if(rowIndex >= getRowCount()){
        return false;
    }
    if(isSingleType()){
        return array[rowIndex]->getValue(colIndex, result);
    } else{
        return anyArray[rowIndex]->getValue(colIndex, result);
    }
}
bool SkMemoryMatrix::setValue(int rowIndex, int colIndex, double val) {
    if(rowIndex >= getRowCount()){
        return false;
    }
    if(isSingleType()){
        return array[rowIndex]->setValue(colIndex, val);
    } else{
        return anyArray[rowIndex]->setValue(colIndex, val);
    }
}
const char* SkMemoryMatrix::getTypes() {
    if(isSingleType()){
        return array[0]->_types;
    }
    return anyArray[0]->_types;
}
SkMemory* SkMemoryMatrix::extractColumn(int columnIndex, SkMemory *out) {
    if(columnIndex >= getColumnCount()){
        return nullptr;
    }
    if(out == nullptr){
        const char *types = getTypes();
        out = SkMemory::create(types[columnIndex % strlen(types)], getRowCount());
    }
    copyData(out, columnIndex);
    return out;
}

#define MAT_OP(op, pType) \
SkMemoryMatrix* SkMemoryMatrix::_mul(pType val) { \
    auto mat = new SkMemoryMatrix(getRowCount()); \
    if(isSingleType()){ \
        for (int i = 0; i < mat->count; ++i) { \
            mat->array[i] = array[i]->_mul(val); \
        } \
    } else{ \
        for (int i = 0; i < mat->count; ++i) { \
            mat->anyArray[i] = anyArray[i]->_mul(val); \
        } \
    } \
    return mat; \
}
MAT_OP(_mul, double)

SkMemoryMatrix* SkMemoryMatrix::_mul(SkMemory *val) {
    if(getColumnCount() != 1){
        return nullptr;
    }
    const int len_val = val->getLength();
    const char outType = MemoryUtils::computeType(val->_types[0], getTypes()[0]);
    const int outUnitSize = MemoryUtils::getUnitSize(outType);
    
    SkMemoryMatrix* mat = new SkMemoryMatrix();
    mat->count = count;
    mat->array = new SkMemory*[count];
    mat->anyArray = nullptr;
    
    const char dstType = val->_types[0];
    const int dstUnitSize = MemoryUtils::getUnitSize(dstType);
    const bool singleType = isSingleType();
    
    SkMemory* cur_mem;
    SkMemory* out_mem;
    SkAnyMemory * cur_mem_any;
    double value;

#define WRITE_OUT_MEM(mem) \
    for (int j = 0; j < len_val; ++j) { \
        value = MemoryUtils::multiple(mem->data, mem->_types[0], 0,\
                                      val->data, dstType , j * dstUnitSize); \
        MemoryUtils::setValue(out_mem->data, outType, j * outUnitSize,value); \
    }

    for (int i = 0, c = getRowCount(); i < c; ++i) {
        mat->array[i] = out_mem = SkMemory::create(outType, len_val);
        //only have one column
        if(singleType){
            cur_mem = array[i];
            //row i * column j(0 - outColumnCount)
            WRITE_OUT_MEM(cur_mem);
        }else{
            cur_mem_any = anyArray[i];
            WRITE_OUT_MEM(cur_mem_any);
        }
    }
    return mat;
}
SkMemoryMatrix* SkMemoryMatrix::_mul(SkAnyMemory *val) {
    if(getColumnCount() != 1){
        return nullptr;
    }
    const int len_val = val->getLength();
    const char outType = MemoryUtils::computeType(MemoryUtils::computeType(val->_types), getTypes()[0]);
    const int outUnitSize = MemoryUtils::getUnitSize(outType);

    const char *dstTypes = val->_types;
    const size_t dstTypeLen = strlen(val->_types);

    const bool singleType = isSingleType();
    SkMemoryMatrix* mat = new SkMemoryMatrix();
    mat->count = count;
    mat->array = new SkMemory*[count];
    mat->anyArray = nullptr;

    SkMemory* cur_mem;
    SkMemory* out_mem;
    SkAnyMemory * cur_mem_any;
    double value;

    char dstType;
    size_t dstBytesIndex = 0;

#define WRITE_OUT_MEM_ANY(mem) \
    for (int j = 0; j < len_val; ++j) { \
        dstType = dstTypes[j % dstTypeLen]; \
        value = MemoryUtils::multiple(mem->data, mem->_types[0], 0, \
                                      val->data, dstType , dstBytesIndex); \
        MemoryUtils::setValue(out_mem->data, outType, j * outUnitSize, value); \
        dstBytesIndex += MemoryUtils::getUnitSize(dstType); \
    }

    for (int i = 0, c = getRowCount(); i < c; ++i) {
        mat->array[i] = out_mem = SkMemory::create(outType, len_val);
        //only have one column
        if(singleType){
            cur_mem = array[i];
            //row i * column j(0 - outColumnCount)
            WRITE_OUT_MEM_ANY(cur_mem);
        } else{
            cur_mem_any = anyArray[i];
            //row i * column j(0 - outColumnCount)
            WRITE_OUT_MEM_ANY(cur_mem_any);
        }
    }
    return mat;
}

SkMemoryMatrix* SkMemoryMatrix::_mul(SkMemoryMatrix *val) {
    //mis match for mat multiple
    if(getColumnCount() != val->getRowCount()){
        return nullptr;
    }
    const char* ts1 = getTypes();
    const char* ts2 = val->getTypes();
    const char outType = MemoryUtils::computeType(MemoryUtils::computeType(ts1),
                                                  MemoryUtils::computeType(ts2));
    const int outUnitSize = MemoryUtils::getUnitSize(outType);
    const int outColumnCount = val->getColumnCount();

    SkMemoryMatrix* mat = new SkMemoryMatrix();
    mat->count = count;
    mat->array = new SkMemory*[count];
    mat->anyArray = nullptr;
    //check type
    if(isSingleType()){
        const char cur_type = array[0]->_types[0];
        const int cur_unitSize = MemoryUtils::getUnitSize(cur_type);
        if(val->isSingleType()){

            const char dst_type = val->array[0]->_types[0];
            const int dst_unitSize = MemoryUtils::getUnitSize(dst_type);

            double value;
            SkMemory* out_mem;
            size_t dst_bytesIndex;
            for (int i = 0, ic = getRowCount(); i < ic; ++i) {
                mat->array[i] = out_mem = SkMemory::create(outType, outColumnCount);
                SkMemory *pMemory = array[i];
                //(0, 0) *(0, 0) + (0, 1) * (1, 0) + (0, 2) * (2, 0)
                for (int k = 0; k < outColumnCount; ++k) {
                    value = 0;
                    dst_bytesIndex = k * dst_unitSize;;
                    for (int j = 0, cc = pMemory->getLength(); j < cc; ++j) {
                        value += MemoryUtils::multiple(pMemory->data, cur_type, j * cur_unitSize,
                                                       val->array[k]->data, dst_type, dst_bytesIndex);
                    }
                    MemoryUtils::setValue(out_mem->data, outType, k * outUnitSize, value);
                }
            }
        } else{
            double value;
            SkMemory* out_mem;

            size_t bytesIndex;
            char dst_type = 0;
            SkAnyMemory *dst_mem;
            SkMemory *cur_mem;
            for (int i = 0, ic = getRowCount(); i < ic; ++i) {
                mat->array[i] = out_mem = SkMemory::create(outType, outColumnCount);
                cur_mem = array[i];

                bytesIndex = 0;
                //(0, 0) *(0, 0) + (0, 1) * (1, 0) + (0, 2) * (2, 0)
                for (int k = 0; k < outColumnCount; ++k) {
                    value = 0;
                    for (int j = 0, cc = cur_mem->getLength(); j < cc; ++j) {
                        dst_mem = val->anyArray[k];
                        const char *dst_types = dst_mem->_types;
                        //type by index i
                        dst_type = dst_types[i % strlen(dst_types)];
                        value += MemoryUtils::multiple(cur_mem->data, cur_type, j * cur_unitSize,
                                                       dst_mem->data, dst_type, bytesIndex);
                    }
                    SkASSERT(dst_type != 0);
                    bytesIndex += MemoryUtils::getUnitSize(dst_type);
                    MemoryUtils::setValue(out_mem->data, outType, k * outUnitSize, value);
                }
            }
        }
    } else{
        //*  *  *    *  *
        //*  *  *    *  *
        //           *  *
        if(val->isSingleType()){
            const char dst_type = val->array[0]->_types[0];
            const int dst_unitSize = MemoryUtils::getUnitSize(dst_type);

            double value;
            SkMemory* outMem;

            char cur_type;
            size_t cur_type_len;
            size_t cur_bytesIndex;
            SkAnyMemory * cur_mem;

            char dst_Type = 0;
            size_t dst_bytesIndex;
            SkMemory * dst_mem;

            for (int i = 0, c = getRowCount(); i < c; ++i) {
                mat->array[i] = outMem = SkMemory::create(outType, outColumnCount);

                cur_mem = anyArray[i];
                const char *cur_types = cur_mem->_types;
                cur_type_len = strlen(cur_types);

                //(0, 0) *(0, 0) + (0, 1) * (1, 0) + (0, 2) * (2, 0)
                for (int k = 0; k < outColumnCount; ++k) {
                    value = 0;
                    dst_bytesIndex = k * dst_unitSize;
                    cur_bytesIndex = 0;
                    for (int j = 0, cc = cur_mem->getLength(); j < cc; ++j) {
                        cur_type = cur_types[j % cur_type_len];

                        dst_mem = val->array[j];
                        value += MemoryUtils::multiple(cur_mem->data, cur_type, cur_bytesIndex,
                                                       dst_mem->data, dst_Type,  dst_bytesIndex);

                        cur_bytesIndex += MemoryUtils::getUnitSize(cur_type);
                    }
                    SkASSERT(dst_Type != 0);
                    MemoryUtils::setValue(outMem->data, outType, k * outUnitSize, value);
                }
            }
        } else{
            double value;
            SkMemory* outMem;

            char cur_type;
            size_t cur_type_len;
            size_t cur_bytesIndex;
            SkAnyMemory * cur_mem;

            char dst_Type = 0;
            size_t dst_bytesIndex = 0;
            SkAnyMemory * dst_mem;

            for (int i = 0, c = getRowCount(); i < c; ++i) {
                mat->array[i] = outMem = SkMemory::create(outType, outColumnCount);
                cur_mem = anyArray[i];

                const char *cur_types = cur_mem->_types;
                cur_type_len = strlen(cur_types);

                dst_bytesIndex = 0;
                //(0, 0) *(0, 0) + (0, 1) * (1, 0) + (0, 2) * (2, 0)
                for (int k = 0; k < outColumnCount; ++k) {
                    value = 0;
                    cur_bytesIndex = 0;

                    for (int j = 0, cc = cur_mem->getLength(); j < cc; ++j) {
                        cur_type = cur_types[j % cur_type_len];

                        dst_mem = val->anyArray[j];
                        const char *dst_types = dst_mem->_types;
                        //type by index i
                        dst_Type = dst_types[i % strlen(dst_types)];
                        value += MemoryUtils::multiple(cur_mem->data, cur_type, cur_bytesIndex,
                                                       dst_mem->data, dst_Type, dst_bytesIndex);

                        cur_bytesIndex += MemoryUtils::getUnitSize(cur_type);
                    }
                    SkASSERT(dst_Type != 0);
                    dst_bytesIndex += MemoryUtils::getUnitSize(dst_Type);

                    MemoryUtils::setValue(outMem->data, outType, k * outUnitSize, value);
                }
            }
        }
    }
    return mat;
}

SkMemoryMatrix* SkMemoryMatrix::copy() {
    auto mat = new SkMemoryMatrix(getRowCount());
    if(isSingleType()){
        for (int i = 0; i < mat->count; ++i) {
            mat->array[i] = array[i]->copy();
        }
    } else{
        for (int i = 0; i < mat->count; ++i) {
            mat->anyArray[i] = anyArray[i]->copy();
        }
    }
    return mat;
}
SkMemoryMatrix * SkMemoryMatrix::transpose() {
    const int columnCount = getColumnCount();
    const int rowCount = getRowCount();
    auto mat = new SkMemoryMatrix(columnCount);

    if(isSingleType()){
        const char* t = array[0]->_types;

        for (int i = 0; i < columnCount; ++i) {
            auto pMemory = SkMemory::create(t, rowCount);
            copyData(pMemory, i);
            mat->array[i] = pMemory;
        }
    } else{
        char outType = MemoryUtils::computeType(anyArray[0]->_types);

        for (int i = 0; i < columnCount; ++i) {
            auto pMemory = SkMemory::create(outType, rowCount);
            copyData(pMemory, i);
            mat->array[i] = pMemory;
        }
    }
    return mat;
}
double SkMemoryMatrix::determinant() {
    //should call isSquareMatrix() before.
    if(getRowCount() == 1){
        if(isSingleType()){
            char type = array[0]->getTypes()[0];
            return MemoryUtils::getValue(array[0]->data, type, 0);
        } else{
            char type = anyArray[0]->getTypes()[0];
            return MemoryUtils::getValue(anyArray[0]->data, type, 0);
        }
    }
    if(getRowCount() == 2){
        if(isSingleType()){
            //{{5, 1}, {2, 3}}
            char type = array[0]->getTypes()[0];
            int unitSize = MemoryUtils::getUnitSize(type);
            double val1 = MemoryUtils::getValue(array[0]->data, type, 0) *
                          MemoryUtils::getValue(array[1]->data, type, unitSize);
            double val2 = MemoryUtils::getValue(array[0]->data, type, unitSize) *
                          MemoryUtils::getValue(array[1]->data, type, 0);
            return val1 - val2;
        } else{
            char t1 = array[0]->getTypes()[1];
            char t2 = array[0]->getTypes()[2];
            int unitSize = MemoryUtils::getUnitSize(t1);

            double val1 = MemoryUtils::getValue(anyArray[0]->data, t1, 0) *
                          MemoryUtils::getValue(anyArray[1]->data, t2, unitSize);
            double val2 = MemoryUtils::getValue(anyArray[0]->data, t2, unitSize) *
                          MemoryUtils::getValue(anyArray[1]->data, t1, 0);
            return val1 - val2;
        }
    } else{
        const int lastRc = getRowCount() - 1;
        const int cc = getColumnCount();
        double val = 0;
        for (int i = 0; i < cc; ++i) {
            val += remainderValue(lastRc, i);
        }
        return val;
    }
}
//remainder-value * $cur_val
double SkMemoryMatrix::remainderValue(size_t rowIndex, size_t columnIndex) {
    double curVal;
    if(isSingleType()){
        char type = array[rowIndex]->getTypes()[0];
        curVal = MemoryUtils::getValue(array[rowIndex]->data, type,
                MemoryUtils::getUnitSize(type) * columnIndex);
    } else{
        if(!anyArray[rowIndex]->getValue(columnIndex, &curVal)){
            LOGW("getValue failed from index = %d", columnIndex);
        }
    }

    SkMemoryMatrix *mat = remainderMat(rowIndex, columnIndex);
    double val = mat->determinant() * curVal;
    mat->unRefAndDestroy();
    if(val == 0){
        return 0;
    }
    return (rowIndex + columnIndex ) % 2 == 0 ? val : -val;
}
SkMemoryMatrix* SkMemoryMatrix::remainderMat(size_t rowIndex, size_t columnIndex) {
    if(getRowCount() != getColumnCount()){
        return nullptr;
    }
    int rc = getRowCount();
    SkMemoryMatrix* mat = new SkMemoryMatrix(rc - 1, true);
    int index = 0;
    for (int i = 0; i < rc; ++i) {
        if(i == rowIndex){
            continue;
        }
        if(isSingleType()){
            mat->array[index ++] = array[i]->kickOut(columnIndex);
        } else{
            mat->array[index ++] = anyArray[i]->kickOut(columnIndex);
        }
    }
    return mat;
}
SkMemoryMatrix* SkMemoryMatrix::algebraicRemainderMat() {
    if(getRowCount() != getColumnCount()){
        return nullptr;
    }
    const char outType = MemoryUtils::computeType(getTypes());
    const int unitSize = MemoryUtils::getUnitSize(outType);
    
    const int rc = getRowCount();
    const int cc = getColumnCount();
    SkMemoryMatrix* mat = new SkMemoryMatrix(rc, true);
    SkMemory* mem;
    double val;
    for (int i = 0; i < rc; ++i) {
        mat->array[i] = mem = SkMemory::create(outType, cc);
        for (int j = 0; j < cc; ++j) {
            val = remainderValue(i, j);
            MemoryUtils::setValue(mem->data, outType, j * unitSize, val);
        }
    }
    return mat;
}
//https://www.shuxuele.com/algebra/matrix-inverse-minors-cofactors-adjugate.html
SkMemoryMatrix* SkMemoryMatrix::adjointMat() {
    if(getRowCount() != getColumnCount()){
        return nullptr;
    }
    //1, 代数余子式矩阵
    //2, 沿正对角线交换位置. 对角线不变
    SkMemoryMatrix *mat = algebraicRemainderMat();
    int rc = mat->getRowCount();
    //we need swap the element count = 'rc * (rc - 1) / 2'
    for(int i = 0; i < rc ; i ++){
        for (int j = i + 1; j < rc; ++j) {
            mat->swapValue(i, j, j, i);
        }
    }
    return mat;
}
SkMemoryMatrix* SkMemoryMatrix::extractMat(size_t rowStart, size_t rowEnd, size_t columnStart,
                                       size_t columnEnd) {
    if(rowEnd > getRowCount() || columnEnd > getColumnCount() || rowEnd <= rowStart || columnEnd <= columnStart){
        return nullptr;
    }
    auto mat = new SkMemoryMatrix(rowEnd - rowStart);
    if(isSingleType()){
        for (size_t i = rowStart; i < rowEnd; ++i) {
            mat->array[i- rowStart] = array[i]->extract(columnStart, columnEnd);
        }
    } else{
        for (size_t i = rowStart; i < rowEnd; ++i) {
            mat->array[i- rowStart] = anyArray[i]->extract(columnStart, columnEnd);
        }
    }
    return mat;
}
SkMemoryMatrix* SkMemoryMatrix::reshape(int rowCount, int colCount, char type, double defVal) {
    if(type == DEF_RESHAPE_TYPE){
        type = MemoryUtils::computeType(getTypes());
    }
    auto mat = new SkMemoryMatrix(rowCount);
    for (int i = 0; i < rowCount; ++i) {
        if(isSingleType()){
            mat->array[i] = array[i]->reshape(colCount, type, defVal);
        } else{
            mat->array[i] = anyArray[i]->reshape(colCount, type, defVal);
        }
    }
    return mat;
}
//-------------------------- start internal ------------------------
static inline SkMemory *
concatMat(SkMemoryMatrix *mat1, SkMemoryMatrix *mat2, int row, int colCount, char type, double defVal) {
    if(mat1->isSingleType()){
        if(mat2->isSingleType()){
            return mat1->array[row]->concat(mat2->array[row], colCount, type, defVal);
        } else{
            return mat1->array[row]->concat(mat2->anyArray[row], colCount, type, defVal);
        }
    } else{
        if(mat2->isSingleType()){
            return mat1->anyArray[row]->concat(mat2->array[row], colCount, type, defVal);
        } else{
            return mat1->anyArray[row]->concat(mat2->anyArray[row], colCount, type, defVal);
        }
    }
}
static inline SkMemory *
concatVal(SkMemoryMatrix *mat1, double defVal, int row, int colCount, char type) {
    if(mat1->isSingleType()){
        return mat1->array[row]->reshape(colCount, type, defVal);
    } else{
        return mat1->anyArray[row]->reshape(colCount, type, defVal);
    }
}
static inline SkMemory *
concatVal2(double defVal, SkMemoryMatrix *mat1, int row, int colCount, char type) {
    if(mat1->isSingleType()){
        return mat1->array[row]->reshapeBefore(colCount, type, defVal);
    } else{
        return mat1->anyArray[row]->reshapeBefore(colCount, type, defVal);
    }
}
//-------------------------- end internal ------------------------

SkMemoryMatrix* SkMemoryMatrix::concat(SkMemoryMatrix *oth, bool vertical, double defVal){
    if(oth == NULL){
        return NULL;
    }
    const bool src_simple = isSingleType();
    const bool oth_simple = oth->isSingleType();
    auto outType = MemoryUtils::computeType(MemoryUtils::computeType(getTypes()),
                                            MemoryUtils::computeType(oth->getTypes()));
    const int curRc = getRowCount();
    const int othRc = oth->getRowCount();
    const int maxRc = curRc >= othRc ? curRc : othRc;

    SkMemoryMatrix* mat;
    if(vertical){
        mat = new SkMemoryMatrix(getRowCount() + oth->getRowCount());
        for (int i = 0; i < mat->getRowCount(); ++i) {
            if(i < curRc){
                if(src_simple){
                    mat->array[i] = array[i]->reshape(maxRc, outType, defVal);
                } else{
                    mat->array[i] = anyArray[i]->reshape(maxRc, outType, defVal);
                }
            } else{
                if(oth_simple){
                    mat->array[i] = oth->array[i - curRc]->reshape(maxRc, outType, defVal);
                } else{
                    mat->array[i] = oth->anyArray[i - curRc]->reshape(maxRc, outType, defVal);
                }
            }
        }
    } else{
        const int resultCC = getColumnCount() + oth->getColumnCount();

        mat = new SkMemoryMatrix(maxRc);
        for (int i = 0; i < mat->getRowCount(); ++i) {
            if(curRc > i){
                if(othRc > i){
                    mat->array[i] = concatMat(this, oth, i, resultCC, outType, defVal);
                } else{
                    mat->array[i] = concatVal(this, defVal, i, resultCC, outType);
                }
            } else{
                //cur out of range.
                if(othRc > i){
                    mat->array[i] = concatVal2(defVal, oth, i, resultCC, outType);
                } else{
                    //never return here
                    mat->unRefAndDestroy();
                    return nullptr;
                }
            }
        }
    }
    return mat;
}
SkMemoryMatrix * SkMemoryMatrix::fliplr(bool copy) {
    const auto rc = getRowCount();
    const bool single = isSingleType();
    if(single && !copy){
        //single type. data can just swap
        for (int i = 0; i < rc; ++i) {
            array[i]->flip(false);
        }
        return this;
    } else {
        auto mat = new SkMemoryMatrix(rc, true);
        for (int i = 0; i < rc; ++i) {
            mat->array[i] = single ? array[i]->flip(copy) : anyArray[i]->flip();
        }
        return mat;
    }
}
SkMemoryMatrix * SkMemoryMatrix::flipud(bool copy) {
    const auto rc = getRowCount();
    const bool single = isSingleType();
    if(copy){
        auto mat = new SkMemoryMatrix(rc, isSingleType());
        for (int i = 0; i < rc; ++i) {
            if(single){
                mat->array[i] = array[rc - i - 1];
            } else{
                mat->anyArray[i] = anyArray[rc - i - 1];
            }
        }
        return mat;
    } else{
        SkMemory* up;
        SkAnyMemory* up2;
        for (int i = 0, c = getRowCount() / 2; i < c; ++i) {
            if(single){
                up = array[i];
                array[i] = array[rc - i - 1];
                array[rc - i - 1] = up;
            } else{
                up2 = anyArray[i];
                anyArray[i] = anyArray[rc - i - 1];
                anyArray[rc - i - 1] = up2;
            }
        }
        return this;
    }
}
//取斜线元素组成数组.
SkMemory* SkMemoryMatrix::diag(int k) {
    if(!isSquareMatrix()){
        return nullptr;
    }
    auto n = getRowCount();
    //getLength() - 1: 主对角线 index
    //row-column
    //--- (0,1), (0,2), (0,3)...(0,n)
    //----(1,1), (1,2), (1,3)...(1,n)
    //----...
    //从左下角。n行0列开始，
    //colIndex, rowIndex
    int lt[2];
    int rb[2];
    if(k > 0){
        lt[0] = k;
        lt[1] = 0;
        rb[0] = n - 1;
        rb[1] = n - 1 - k;
    } else{
        lt[0] = 0;
        lt[1] = -k;
        rb[0] = n - 1 + k;
        rb[1] = n - 1;
    }
    //create mem
    const int count = n - abs(k);
    auto rt = MemoryUtils::computeType(getTypes());
    auto out_unitSize = MemoryUtils::getUnitSize(rt);
    auto outMem = SkMemory::create(rt, count);
    //set data
    double val;
    for (int i = 0; i < count; ++i) {
        getValue(lt[1] + i, lt[0] + i, &val);
        MemoryUtils::setValue(outMem->data, rt, out_unitSize * i, val);
    }
    return outMem;
}
SkMemoryMatrix* SkMemoryMatrix::_triul(bool up, int k, double defVal) {
    if(!isSquareMatrix()){
        return nullptr;
    }
    auto n = getRowCount();
    //getLength() - 1: 主对角线 index
    const int index = n - 1 + k; //include index
    if(index > 2*n - 1){
        return nullptr;
    }
    //要改变的数据Index
    //从左下角。n行0列开始，
    //colIndex, rowIndex
    int lt[2] = {0, n - 1 - index};
    int rb[2] = {n - 1 - index, n - 1};
    //init mat
    auto mat = new SkMemoryMatrix(n, true);
    auto rt = MemoryUtils::computeType(getTypes());
    for (int i = 0; i < n; ++i) {
        mat->array[i] = SkMemory::create(rt, n);
    }
    //set value
    HMath::LinearEquation line(lt[0], lt[1], rb[0], rb[1]);
    double val;
    if(up){
        for (int i = 0; i < n; ++i) { //col
            for (int j = 0; j < n; ++j) { //row
                if(line.getLocation(i, j) != -1){
                    getValue(j, i, &val);
                } else{
                    val = defVal;
                }
                mat->array[j]->setValue(i, val);
            }
        }
    } else{
        for (int i = 0; i < n; ++i) { //col
            for (int j = 0; j < n; ++j) { //row
                if(line.getLocation(i, j) != 1){
                    getValue(j, i, &val);
                } else{
                    val = defVal;
                }
                mat->array[j]->setValue(i, val);
            }
        }
    }
    return mat;
}
//上三角矩阵的抽取, 对角线 从左上到右下
//抽取矩阵中第k条对角线及其以上的元素。k=0指主对角线，k>0指主对角线以上的第k条对角线，k<0指主对角线以下的第k条对角线。
SkMemoryMatrix * SkMemoryMatrix::triu(int k, double defVal) {
    return this->_triul(true, k, defVal);
}
SkMemoryMatrix* SkMemoryMatrix::tril(int k, double defVal) {
    return this->_triul(false, k, defVal);
}

int SkMemoryMatrix::inverse(lua_State* L) {
    if(getRowCount() != getColumnCount()){
        return 0; // doesn't support
    }
    auto in = convert(L, "F");
    if(!in){
        return 0;
    }
    const char rt = 'F';
    auto unitSize = MemoryUtils::getUnitSize(rt);

    auto mat = LuaUtils::get_ref<SkMemoryMatrix>(L, -1);
    auto func_set = [&](int id1, int id2, double val){
        MemoryUtils::setValue(mat->array[id1]->data, rt, id2 * unitSize, val);
    };
    auto func_get = [&](int id1, int id2){
        return MemoryUtils::getValue(mat->array[id1]->data, rt, id2 * unitSize);
    };
    auto result = MathUtils::inverse(getRowCount(), func_set, func_get);
    if(result){
        return 1;
    }
    lua_pop(L, 1);
    mat->unRefAndDestroy();
    return 0;
}

int SkMemoryMatrix::convert(lua_State* L,const char *ts) {
    if(isSingleType()){
        if(strlen(ts) == 1){
            //simple -> simple
            const auto mat = new SkMemoryMatrix(getRowCount());
            for (int i = 0; i < getRowCount(); ++i) {
                if(array[i]->convert(L, ts) == 0){
                    //error
                    mat->unRefAndDestroy();
                    return 0;
                }
                mat->array[i] = LuaUtils::get_ref<SkMemory>(L, -1);
            }
            LuaUtils::push_ptr(L, mat);
            return 1;
        }else{
            //simple -> lazy
            const auto mat = new SkMemoryMatrix(getRowCount(), false);
            for (int i = 0; i < getRowCount(); ++i) {
                if(array[i]->convert(L, ts) == 0){
                    //error
                    mat->unRefAndDestroy();
                    return 0;
                }
                mat->anyArray[i] = LuaUtils::get_ref<SkAnyMemory>(L, -1);
            }
            LuaUtils::push_ptr(L, mat);
            return 1;
        }
    }else{
        //lazy -> simple
        if(strlen(ts) == 1){
            const auto mat = new SkMemoryMatrix(getRowCount());
            for (int i = 0; i < getRowCount(); ++i) {
                if(anyArray[i]->convert(L, ts) == 0){
                    //error
                    mat->unRefAndDestroy();
                    return 0;
                }
                mat->array[i] = LuaUtils::get_ref<SkMemory>(L, -1);
            }
            LuaUtils::push_ptr(L, mat);
            return 1;
        } else{
            //lazy -> lazy
            const auto mat = new SkMemoryMatrix(getRowCount(), false);
            for (int i = 0; i < getRowCount(); ++i) {
                if(anyArray[i]->convert(L, ts) == 0){
                    //error
                    mat->unRefAndDestroy();
                    return 0;
                }
                mat->anyArray[i] = LuaUtils::get_ref<SkAnyMemory>(L, -1);
            }
            LuaUtils::push_ptr(L, mat);
            return 1;
        }
    }
}
//collect column data to array.
void SkMemoryMatrix::copyData(SkMemory *pMemory, int columnIndex) {
    //make every column to a new row
    if(isSingleType()){
        for (int i = 0; i < count; ++i) {
            array[i]->writeTo(pMemory, i, columnIndex);
        }
    } else{
        const char dstType = pMemory->_types[0];
        const int dstUnitSize = MemoryUtils::getUnitSize(dstType);
        //all column
        const char *types = anyArray[0]->_types;
        const char type = types[columnIndex % strlen(types)];
        const size_t bytesIndex = MemoryUtils::computeBytesIndex(types, columnIndex);

        double val;
        for (int i = 0; i < count; ++i) {
            val = MemoryUtils::getValue(anyArray[i]->data, type, bytesIndex);
            MemoryUtils::setValue(pMemory->data, pMemory->_types[0], i * dstUnitSize, val);
        }
    }
}

void SkMemoryMatrix::swapValue(int rowIndex1, int colIndex1, int rowIndex2, int colIndex2) {
    if(isSingleType()){
        char t = getTypes()[0];
        //array[rowIndex1]
        SkMemory *mem1 = array[rowIndex1];
        SkMemory *mem2 = array[rowIndex2];
        size_t offset1 = MemoryUtils::getUnitSize(t) * colIndex1;
        size_t offset2 = MemoryUtils::getUnitSize(t) * colIndex2;

        double tmpVal = MemoryUtils::getValue(mem1->data, t, offset1);
        MemoryUtils::setValue(mem1->data, t, offset1,
                              MemoryUtils::getValue(mem2->data, t, offset2)
        );
        MemoryUtils::setValue(mem2->data, t, offset2, tmpVal);
    } else{
        const auto *mem1 = anyArray[rowIndex1];
        const auto *mem2 = array[rowIndex2];
        const size_t offset1 = MemoryUtils::computeBytesIndex(getTypes(), colIndex1);
        const size_t offset2 = MemoryUtils::computeBytesIndex(getTypes(), colIndex2);
        const size_t len = strlen(getTypes());
        const char type1 = getTypes()[colIndex1 % len];
        const char type2 = getTypes()[colIndex2 % len];

        double tmpVal = MemoryUtils::getValue(mem1->data, type1, offset1);
        MemoryUtils::setValue(mem1->data, type1, offset1,
                              MemoryUtils::getValue(mem2->data, type2, offset2)
        );
        MemoryUtils::setValue(mem2->data, type2, offset2, tmpVal);
    }
}
//get the value row by row
bool SkMemoryMatrix::getValueRowByRow(int index, double *outVal) {
    if(index < 0 || index >= getRowCount() * getColumnCount()){
        return false;
    }
    const auto cc = getColumnCount();
    return getValue(index / cc, index % cc, outVal);
}
//get the value column by column
bool SkMemoryMatrix::getValueColByCol(int index, double *outVal) {
    if(index < 0 || index >= getRowCount() * getColumnCount()){
        return false;
    }
    const auto cc = getRowCount();
    return getValue(index % cc, index / cc, outVal);;
}
SkMemoryMatrix* SkMemoryMatrix::mergeUnit(char dstType, size_t unitCount,
                                          Func_Merge Merge, void* ctx, const char** outError) {
    auto rc = getRowCount();
    SkMemoryMatrix * result = new SkMemoryMatrix(rc, true);
    if(isSingleType()){
        for (int i = 0; i < rc; ++i) {
            result->array[i] = array[i]->mergeUnit(dstType, unitCount, Merge, ctx, outError);
            if(*outError != NULL){
                goto fail;
            }
        }
    } else{
        for (int i = 0; i < rc; ++i) {
            result->array[i] = anyArray[i]->mergeUnit(dstType, unitCount, Merge, ctx, outError);
            if(*outError != NULL){
                goto fail;
            }
        }
    }
    return result;
fail:
    result->unRefAndDestroy();
    return NULL;
}

SkMemoryMatrix* SkMemoryMatrix::splitUnit(char dstType, size_t unitCount,
                                          Func_Split Split, void* ctx, const char** outError) {
    auto rc = getRowCount();
    SkMemoryMatrix * result = new SkMemoryMatrix(rc, true);
    if(isSingleType()){
        for (int i = 0; i < rc; ++i) {
            result->array[i] = array[i]->splitUnit(dstType, unitCount, Split, ctx, outError);
            if(*outError != NULL){
                goto fail;
            }
        }
    } else{
        for (int i = 0; i < rc; ++i) {
            result->array[i] = anyArray[i]->splitUnit(dstType, unitCount, Split, ctx, outError);
            if(*outError != NULL){
                goto fail;
            }
        }
    }
    return result;
fail:
    result->unRefAndDestroy();
    return NULL;
}
