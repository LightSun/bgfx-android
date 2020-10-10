//
// Created by Administrator on 2020/8/27 0027.
//
#include <SkUTF.h>

#include "printer.h"
#include "SkMemory.h"
#include "common.h"
#include "LuaUtils.h"
#include "MatUtils.h"

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

#define READ(type) \
type read_##type(SkMemory* mem, int index){ \
auto addr = static_cast<type*>(mem->data); \
addr += index; \
return *addr; \
}
READ(float)
READ(uint8_t)
READ(uint16_t)
READ(uint32_t)
READ(short)
READ(int)
//READ(char)
READ(double)

#define READ_char() \
s_char read_char(SkMemory* mem, int index){ \
auto addr = static_cast<s_char*>(mem->data); \
addr += index; \
return *addr; \
}
READ_char();


#define WRITE(type) \
void write_##type(SkMemory* mem, int index, type val){ \
auto addr = static_cast<type*>(mem->data); \
addr += index; \
*addr = val; \
}
WRITE(float)
WRITE(uint8_t)
WRITE(uint16_t)
WRITE(uint32_t)
WRITE(short)
WRITE(int)
//WRITE(char)
WRITE(double)

#define WRITE_char() \
void write_char(SkMemory *mem, int index, s_char val) { \
    auto addr = static_cast<s_char *>(mem->data); \
    addr += index; \
    *addr = val; \
}
WRITE_char()


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
int SkMemory::write(SkMemory* mem, lua_State *L) {
    //table, index, value
    auto index = lua_tointeger(L, -2);
    if(index >= mem->getLength()){
        return luaL_error(L, "index(%d) out of range(%d).", index, mem->getLength());
    }
    switch (mem->_types[0]) {
        case 'f':
            write_float(mem, index, TO_FLOAT(L, -1));
            return 0;
        case 'd':
            write_uint32_t(mem, index, TO_NUMBER_32(L, -1));
            return 0;
        case 'w':
            write_uint16_t(mem, index, TO_NUMBER_16(L, -1));
            return 0;
        case 'b': {
            write_uint8_t(mem, index, TO_NUMBER_8(L, -1));
            return 0;
        }

        case 's': {
            write_short(mem, index, TO_SHORT(L, -1));
            return 0;
        }
        case 'i': {
            write_int(mem, index, TO_INT(L, -1));
            return 0;
        }
        case 'c': {
            write_char(mem, index, TO_CHAR(L, -1));
            return 0;
        }
        case 'F': {
            write_double(mem, index, TO_DOUBLE(L, -1));
            return 0;
        }
        default:
            return luaL_error(L, "wrong data type = %s", mem->_types);
    }
}
int SkMemory::read(SkMemory* mem, lua_State *L) {
    //table, index
    auto index = lua_tointeger(L, -1);
    if(index >= mem->getLength()){
        return luaL_error(L, "index(%d) out of range(%d).", index, mem->getLength());
    }
    switch (mem->_types[0]) {
        case 'f':
            lua_pushnumber(L, read_float(mem, index));
            return 1;
        case 'd':
            lua_pushnumber(L, read_uint32_t(mem, index));
            return 1;
        case 'w':
            lua_pushnumber(L, read_uint16_t(mem, index));
            return 1;

        case 'b': {
            lua_pushnumber(L, read_uint8_t(mem, index));
            return 1;
        }

        case 's': {
            lua_pushnumber(L, read_short(mem, index));
            return 1;
        }
        case 'i': {
            lua_pushnumber(L, read_int(mem, index));
            return 1;
        }
        case 'c': {
            lua_pushnumber(L, read_char(mem, index));
            return 1;
        }

        case 'F': {
            lua_pushnumber(L, read_double(mem, index));
            return 1;
        }

        default:
            return luaL_error(L, "wrong data type = %s", mem->_types);
    }
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

        MemoryUtils::write(pMemory->data, outType, j* outUnitSize, srcVal * dstVal);
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
        MemoryUtils::write(out_mem->data, outType, j* outUnitSize, srcVal * dstVal);
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
            MemoryUtils::write(out_mem->data, outType, i * outUnitSize, srcVal * value);
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
            MemoryUtils::write(out_mem->data, outType, i * outUnitSize, srcVal * value);
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
            MemoryUtils::write(out_mem->data, outType, i * outUnitSize, value);
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
            MemoryUtils::write(out_mem->data, outType, i * outUnitSize, total);
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
    SkMemory *pMemory = SkMemory::create(_types, getLength()-1);
    const int unitSize = MemoryUtils::getUnitSize(_types[0]);
    unsigned char* da = static_cast<unsigned char *>(data);
    if(index > 0){
        memcpy(pMemory->data, (void*)da, unitSize * index);
    }
    da += unitSize * (index + 1);
    memcpy(pMemory->data, (void*)da, (pMemory->getLength() - index) * unitSize);
    return pMemory;
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

            MemoryUtils::write(out_mem->data, outType, i * outUnitSize, srcVal * value);
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

            MemoryUtils::write(out_mem->data, outType, i * outUnitSize, srcVal * value);
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
            MemoryUtils::write(pMemory->data, outType, i * outUnitSize, value);
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
            MemoryUtils::write(pMemory->data, outType, i * outUnitSize, value);
        }
    }
    return pMemory;
}
double SkAnyMemory::get(size_t index, bool* success) {
    SkAnyMemory* mem = this;
    int bytes = index / mem->_elementCount * mem->size / mem->_tabCount;
    const auto typesLen = strlen(mem->_types);
    char t;
    for (int i = 0, len = index % mem->_elementCount + 1; i < len; ++i) {
        t = mem->_types[i % typesLen];
        //for last we read data to lua stack
        if(i == len - 1){
            if(success){
                *success = true;
            }
            return MemoryUtils::getValue(mem->data, t, bytes);
        }
        bytes += MemoryUtils::getUnitSize(t);
    }
    if(success){
        *success = false;
    }
    return -1;
}
//read data to lua stack
int SkAnyMemory::read(SkAnyMemory *mem, lua_State *L) {
    //table, index
    auto index = lua_tointeger(L, -1);
    if(index >= mem->getLength()){
        return luaL_error(L, "index(%d) out of range(%d).", index, mem->getLength());
    }
    bool success;
    double val = mem->get(index, &success);
    if(success){
        lua_pushnumber(L, val);
        return 1;
    }
    return luaL_error(L, "wrong index");

    //base bytes
    /*int bytes = index / mem->_elementCount * mem->size / mem->_tabCount;
    const auto typesLen = strlen(mem->_types);
    char t;
    for (int i = 0, len = index % mem->_elementCount + 1; i < len; ++i) {
        t = mem->_types[i % typesLen];
        //for last we read data to lua stack
        if(i == len - 1){
            if(MemoryUtils::write(L, t, mem->data, bytes) < 0){
                return 0;
            }
            return 1;
        }
        bytes += MemoryUtils::getUnitSize(t);
    }
    return luaL_error(L, "wrong index");*/
}
int SkAnyMemory::write(SkAnyMemory *mem, lua_State *L) {
    //table, index, value
    auto index = lua_tointeger(L, -2);
    if(index >= mem->getLength()){
        return luaL_error(L, "index(%d) out of range(%d).", index, mem->getLength());
    }
    //base bytes
    int bytes = index / mem->_elementCount * mem->size / mem->_tabCount;
    const auto typesLen = strlen(mem->_types);
    char t;
    for (int i = 0, len = index % mem->_elementCount + 1; i < len; ++i) {
        t = mem->_types[i % typesLen];
        //for last we read data from lua stack
        if(i == len - 1){
            if(MemoryUtils::read(L, t, mem->data, bytes) < 0){
                return 0;
            }
            return 1;
        }
        bytes += MemoryUtils::getUnitSize(t);
    }
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
        MemoryUtils::write(outMem->data, outType, out_unitSize * (i - start), value);
        srcBytes += MemoryUtils::getUnitSize(srcType);
    }
    return outMem;
}

SkMemory *SkAnyMemory::kickOut(size_t index) {
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
            MemoryUtils::write(outMem->data, outType, out_unitSize * i , value);
        } else{
            MemoryUtils::write(outMem->data, outType, out_unitSize * (i - 1) , value);
        }
        srcBytes += MemoryUtils::getUnitSize(srcType);
    }

    return outMem;
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
    ss << "{";
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
        }
    }
    ss << "}";
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
const char* SkMemoryMatrix::getTypes() {
    if(isSingleType()){
        return array[0]->_types;
    }
    return anyArray[0]->_types;
}
SkMemory* SkMemoryMatrix::collectColumn(int columnIndex, SkMemory *out) {
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
        MemoryUtils::write(out_mem->data, outType, j * outUnitSize,value); \
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
        MemoryUtils::write(out_mem->data, outType, j * outUnitSize, value); \
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
                    MemoryUtils::write(out_mem->data, outType,  k *outUnitSize, value);
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
                    MemoryUtils::write(out_mem->data, outType,  k *outUnitSize, value);
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
                    MemoryUtils::write(outMem->data, outType,  k *outUnitSize, value);
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

                    MemoryUtils::write(outMem->data, outType,  k *outUnitSize, value);
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
            double val2 = MemoryUtils::getValue(array[1]->data, type, unitSize) *
                          MemoryUtils::getValue(array[0]->data, type, 0);
            return val1 - val2;
        } else{
            char t1 = array[0]->getTypes()[1];
            char t2 = array[0]->getTypes()[2];
            int unitSize = MemoryUtils::getUnitSize(t1);

            double val1 = MemoryUtils::getValue(anyArray[0]->data, t1, 0) *
                          MemoryUtils::getValue(anyArray[1]->data, t2, unitSize);
            double val2 = MemoryUtils::getValue(anyArray[1]->data, t2, unitSize) *
                          MemoryUtils::getValue(anyArray[0]->data, t1, 0);
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
        curVal = MemoryUtils::getValue(array[rowIndex]->data, type, type * columnIndex);
    } else{
        curVal = anyArray[rowIndex]->get(columnIndex, nullptr);
    }

    SkMemoryMatrix *mat = remainderMat(rowIndex, columnIndex);
    double val = mat->determinant() * curVal;
    mat->unRefAndDestroy();
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
            MemoryUtils::write(mem->data, outType, j * unitSize, val);
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
        MemoryUtils::write(mat->array[id1]->data, rt, id2 * unitSize, val);
    };
    auto func_get = [&](int id1, int id2){
        return MemoryUtils::getValue(mat->array[id1]->data, rt, id2 * unitSize);
    };
    auto result = MatUtils::inverse(getRowCount(), func_set, func_get);
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
            MemoryUtils::write(pMemory->data, pMemory->_types[0], i * dstUnitSize, val);
        }
    }
}

void SkMemoryMatrix::swapValue(int rowIndex1, int colIndex1, int rowIndex2, int colIndex2) {
    char t = getTypes()[0];
    //array[rowIndex1]
    SkMemory *mem1 = array[rowIndex1];
    SkMemory *mem2 = array[rowIndex2];
    size_t offset1 = MemoryUtils::getUnitSize(t) * colIndex1;
    size_t offset2 = MemoryUtils::getUnitSize(t) * colIndex2;

    double tmpVal = MemoryUtils::getValue(mem1->data, t, offset1);
    MemoryUtils::write(mem1->data, t, offset1,
            MemoryUtils::getValue(mem2->data, t, offset2)
    );
    MemoryUtils::write(mem2->data, t, offset2, tmpVal);
}
