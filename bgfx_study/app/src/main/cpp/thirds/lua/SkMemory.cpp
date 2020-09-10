//
// Created by Administrator on 2020/8/27 0027.
//
#include <SkUTF.h>

#include "printer.h"
#include "SkMemory.h"
#include "common.h"
#include "LuaUtils.h"


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
READ(char)
READ(double)

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
WRITE(char)
WRITE(double)

#define COPY_SINGLE_DATA(type) \
{ auto pData = (type *) (dstMem->data); \
auto srcData = (type *) (data); \
pData += dstIndex; \
srcData += srcIndex; \
*pData = *srcData; }

//---------------------------------------------------------------------------------
SkMemory::SkMemory(){

}
SkMemory::SkMemory(const char *type, int len): SimpleMemory(), _dType(type) {
    size = MemoryUtils::getUnitSize(type[0]) * len;
    SkASSERT(size > 0);
    data = malloc(size);
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
            ARRAY_INIT(char);
        }

        case 'F':{
            ARRAY_INIT(double);
        }
            break;
    }
}
//start from 0
SkMemory::SkMemory(lua_State *L, int start, int tableCount, const char *t) : SimpleMemory(){
    _dType = t;
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
            copy_data_i(char);
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
SkMemory::SkMemory(lua_State *L, int tableCount, const char *t) : SkMemory(L, 0, tableCount, t){
}

bool SkMemory::isFloat() {
    return _dType[0] == 'f';
}
int SkMemory::write(SkMemory* mem, lua_State *L) {
    //table, index, value
    auto index = lua_tointeger(L, -2);
    if(index >= mem->getLength()){
        return luaL_error(L, "index(%d) out of range(%d).", index, mem->getLength());
    }
    switch (mem->_dType[0]) {
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
            return luaL_error(L, "wrong data type = %s", mem->_dType);
    }
}
int SkMemory::read(SkMemory* mem, lua_State *L) {
    //table, index
    auto index = lua_tointeger(L, -1);
    if(index >= mem->getLength()){
        return luaL_error(L, "index(%d) out of range(%d).", index, mem->getLength());
    }
    switch (mem->_dType[0]) {
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
            return luaL_error(L, "wrong data type = %s", mem->_dType);
    }
}
void SkMemory::toString(SB::StringBuilder& ss) {
    switch (_dType[0]) {
        case 'f':
            Printer::printArray((float*)data, size / 4, ss);
            break;
        case 'd':
            Printer::printArray((uint32_t*)data, size / 4, ss );
            break;
        case 'w':
            Printer::printArray((uint16_t*)data, size / 2, ss);
            break;
        case 'b':
            Printer::printArray((uint8_t*)data, size, ss);
            break;

        case 's':
            Printer::printArray((short *)data, size, ss);
            break;
        case 'i':
            Printer::printArray((int*)data, size, ss);
            break;
        case 'c':
            Printer::printArray((char*)data, size, ss);
            break;

        case 'F':
            Printer::printArray((double *)data, size, ss);
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
    return size / MemoryUtils::getUnitSize(_dType[0]);
}

void SkMemory::writeTo(SkMemory *dstMem, int dstIndex, int srcIndex) {
    switch (_dType[0]) {
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
        COPY_SINGLE_DATA(char)
            break;

        case 'F':
        COPY_SINGLE_DATA(double)
            break;
    }
}
IMemory* SkMemory::convert(const char* ts) {
    auto dstStrLen = strlen(ts);
    if(dstStrLen == 1){
        if(_dType[0] == ts[0]) return this;
        auto pMemory = new SkMemory();
        pMemory->_dType = ts;
        pMemory->size = MemoryUtils::getUnitSize(ts[0]) * getLength();
        pMemory->data = malloc(pMemory->size);
        for (int i = 0, length = getLength(); i < length; ++i) {
            MemoryUtils::convert(data, _dType[0] ,pMemory->data, ts[0], i);
        }
        return pMemory;
    }else{
        //error mismatch
        if(getLength() % dstStrLen != 0){
            return nullptr;
        }
        size_t tabCount = getLength() / dstStrLen;
        const auto pMemory = new SkAnyMemory(ts, tabCount, false);
        //copy data
        const char srcType = _dType[0];
        const auto unitSize = MemoryUtils::getUnitSize(srcType);
        char dstType;
        size_t  dstBytes = 0;
        for (int i = 0, length = getLength(); i < length; ++i) {
            dstType = ts[i % dstStrLen];
            MemoryUtils::convert(data, srcType, i * unitSize, pMemory->data, dstType, dstBytes);
            dstBytes += MemoryUtils::getUnitSize(dstType);
        }
        return pMemory;
    }
}

//======================= SKAnyMemory ===================================
SkAnyMemory::SkAnyMemory(lua_State *L, const char *types): SkAnyMemory(L, types, -1){

}
SkAnyMemory::SkAnyMemory(lua_State *L, const char *types, int tableIndex){
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
IMemory* SkAnyMemory::convert(const char *ts) {
    const int dstStrLen = strlen(ts);
    if(dstStrLen == 1){
        const int dstUnitSize = MemoryUtils::getUnitSize(ts[0]);
        auto pMemory = new SkMemory();
        pMemory->_dType = ts;
        pMemory->size = dstUnitSize * getLength();
        pMemory->data = malloc(pMemory->size);
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
        return pMemory;
    } else{
        //error mismatch
        if(getLength() % dstStrLen != 0){
            return nullptr;
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
        return pMemory;
    }
}
int SkAnyMemory::read(SkAnyMemory *mem, lua_State *L) {
    //table, index
    auto index = lua_tointeger(L, -1);
    if(index >= mem->getLength()){
        return luaL_error(L, "index(%d) out of range(%d).", index, mem->getLength());
    }
    //base bytes
    int bytes = index / mem->_elementCount * mem->size / mem->_tabCount;
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
    return luaL_error(L, "wrong index");
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

//------------------------ SkMemoryFFFI ----------------------
SkMemoryFFFUI::SkMemoryFFFUI(lua_State *L, int tableCount) : SimpleMemory(){
    size = 4 * 4 * tableCount;
    SkASSERT(size > 0);
    data = malloc(size);

    //LOGD("----- new SkMemoryFFFUI -----");
    float* addr_f = static_cast<float *>(data);
    uint32_t* addr_ui = static_cast<uint32_t *>(data);
    int index = 0;

    int type;
    for (int i = 0; i < tableCount; ++i) {
        for (int idx = 0; idx < 3; ++idx) {
            type = lua_rawgeti(L, i + 1, idx + 1);
            if(type != LUA_TNUMBER){
                luaL_error(L, "create SkMemoryFFFUI failed. for error data type = %d", type);
                goto out;
            }
            *(addr_f + index) = TO_FLOAT(L, -1);
            index ++;
            lua_pop(L, 1);
        }
        type = lua_rawgeti(L, i + 1, 4);
        if(type != LUA_TNUMBER){
            luaL_error(L, "create SkMemoryFFFUI failed. for error data type = %d", type);
            goto out;
        }
        *(addr_ui + index) = TO_NUMBER_32(L, -1);
        index ++;
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    return ;
    out:
        unRef();
}
void SkMemoryFFFUI::toString(SB::StringBuilder &ss) {
    uint32_t * addr_ui = static_cast<uint32_t *>(data);
    float * addr_f = static_cast<float *>(data);

    ss << "[";
    for(int i = 0, len = size / 4 ; i < len ; i ++ ){
        if((i + 1) % 4 == 0){
            ss << addr_ui[i];
        } else{
            ss << addr_f[i];
        }
        if( i != len - 1){
            ss << ", ";
        }
    }
    ss << "]";
}
int SkMemoryFFFUI::getLength() {
    return size / 4;
}
IMemory* SkMemoryFFFUI::convert(const char *ts) {
    const int dstStrLen = strlen(ts);
    if(dstStrLen == 1){
        const int dstUnitSize = MemoryUtils::getUnitSize(ts[0]);
        auto pMemory = new SkMemory();
        pMemory->_dType = ts;
        pMemory->size = dstUnitSize * getLength();
        pMemory->data = malloc(pMemory->size);

        //convert data
        const char* types = "fffd";
        for (int i = 0, len = getLength(); i < len; ++i) {
            MemoryUtils::convert(data, types[i % 4], i * 4, pMemory->data, ts[0], i * dstUnitSize);
        }
        return pMemory;
    } else{
        if(getLength() % dstStrLen != 0){
            return nullptr;
        }
        size_t tabCount = getLength() / dstStrLen;
        const auto pMemory = new SkAnyMemory(ts, tabCount, false);
        //copy data
        const char* srcTypes = "fffd";
        char srcType;
        char dstType;
        size_t dstBytes = 0;
        for (int i = 0, length = getLength(); i < length; ++i) {
            dstType = ts[i % dstStrLen];
            srcType = srcTypes[i % 4];
            MemoryUtils::convert(data, srcType, i * 4, pMemory->data, dstType, dstBytes);
            dstBytes += MemoryUtils::getUnitSize(dstType);
        }
        return pMemory;
    }
}

int SkMemoryFFFUI::read(SkMemoryFFFUI *mem, lua_State *L) {
    //table, index
    auto index = lua_tointeger(L, -1);
    if(index >= mem->getLength()){
        return luaL_error(L, "index(%d) out of range(%d).", index, mem->getLength());
    }
    if((index + 1) % 4 == 0){
        uint32_t * addr = static_cast<uint32_t *>(mem->data);
        lua_pushnumber(L, addr[index]);
    } else{
        float * addr = static_cast<float *>(mem->data);
        lua_pushnumber(L, addr[index]);
    }
    return 1;
}
int SkMemoryFFFUI::write(SkMemoryFFFUI *mem, lua_State *L) {
    //table, index, value
    auto index = lua_tointeger(L, -2);
    if(index >= mem->getLength()){
        return luaL_error(L, "index(%d) out of range(%d).", index, mem->getLength());
    }
    if((index + 1) % 4 == 0){ //ui
        uint32_t * addr = static_cast<uint32_t *>(mem->data);
        addr[index] = TO_NUMBER_32(L, -1);
    } else{
        float * addr = static_cast<float *>(mem->data);
        addr[index] = TO_FLOAT(L, -1);
    }
    return 0;
}
//--------------------- Sk memory matrix --------------------
SkMemoryMatrix::~SkMemoryMatrix() {
    destroyData();
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
SkMemoryMatrix::SkMemoryMatrix(int count, bool singleType): count(count) {
    if(singleType){
        array = new SkMemory*[count];
        anyArray = nullptr;
    } else{
        anyArray = new SkAnyMemory*[count];
        array = nullptr;
    }
}
void SkMemoryMatrix::destroyData() {
    if(array){
        delete[](array);
        array = nullptr;
    }
    if(anyArray){
        delete[](anyArray);
        anyArray = nullptr;
    }
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
        array[i]->toString(ss);
        if (i != count - 1) {
            ss << ",";
        }
    }
    ss << "}";
}

SkMemoryMatrix * SkMemoryMatrix::transpose() {
    if(isSingleType()){
        auto mat = new SkMemoryMatrix(getColumnCount());
        const char* t = array[0]->_dType;
        int size = MemoryUtils::getUnitSize(t[0]) * getRowCount();

        for (int i = 0; i < mat->count; ++i) {
            auto pMemory = new SkMemory();
            pMemory->_dType = t;
            pMemory->size = size;
            pMemory->data = malloc(size);
            copyData(pMemory, i);
            mat->array[i] = pMemory;
        }
        return mat;
    } else{
        //for type of SkAnyMemory doesn't support transpose.
        return nullptr;
    }
}

SkMemoryMatrix * SkMemoryMatrix::convert(const char *ts) {
    if(isSingleType()){
        if(strlen(ts) == 1){
            //simple -> simple
            const auto mat = new SkMemoryMatrix(getRowCount());
            for (int i = 0; i < getRowCount(); ++i) {
                auto pMemory = array[i]->convert(ts);
                if(pMemory == nullptr){
                    mat->destroyAll();
                    delete(mat);
                    return nullptr;
                }
                mat->array[i] = (SkMemory*)pMemory;
            }
            return mat;
        }else{
            //simple -> lazy
            const auto mat = new SkMemoryMatrix(getRowCount(), false);
            for (int i = 0; i < getRowCount(); ++i) {
                auto pMemory = array[i]->convert(ts);
                if(pMemory == nullptr){
                    mat->destroyAll();
                    delete(mat);
                    return nullptr;
                }
                mat->anyArray[i] = (SkAnyMemory*)pMemory;
            }
            return mat;
        }
    }else{
        //lazy -> simple
        if(strlen(ts) == 1){
            const auto mat = new SkMemoryMatrix(getRowCount());
            for (int i = 0; i < getRowCount(); ++i) {
                auto pMemory = anyArray[i]->convert(ts);
                if(pMemory == nullptr){
                    mat->destroyAll();
                    delete(mat);
                    return nullptr;
                }
                mat->array[i] = (SkMemory*)pMemory;
            }
            return mat;
        } else{
            //lazy -> lazy
            const auto mat = new SkMemoryMatrix(getRowCount(), false);
            for (int i = 0; i < getRowCount(); ++i) {
                auto pMemory = anyArray[i]->convert(ts);
                if(pMemory == nullptr){
                    mat->destroyAll();
                    delete(mat);
                    return nullptr;
                }
                mat->anyArray[i] = (SkAnyMemory*)pMemory;
            }
            return mat;
        }
    }
}

void SkMemoryMatrix::copyData(SkMemory *pMemory, int columnIndex) {
    //make every column to a new row
    for (int i = 0; i < count; ++i) {
        array[i]->writeTo(pMemory, i, columnIndex);
    }
}
void SkMemoryMatrix::destroyAll() {
    if(isSingleType()){
        for (int i = 0; i < getRowCount(); ++i) {
            auto pMemory = array[i];
            if(pMemory != nullptr){
                delete pMemory;
            }
        }
        array = nullptr;
    } else{
        for (int i = 0; i < getRowCount(); ++i) {
            auto pMemory = anyArray[i];
            if(pMemory != nullptr){
                delete pMemory;
            }
        }
        anyArray = nullptr;
    }
}