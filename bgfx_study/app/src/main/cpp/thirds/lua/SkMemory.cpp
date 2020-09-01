//
// Created by Administrator on 2020/8/27 0027.
//
#include <SkUTF.h>

#include "printer.h"
#include "SkMemory.h"
#include "common.h"
#include "lua_wrapper.h"

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


AbsSkMemory::~AbsSkMemory() {
    destroyData();
}
AbsSkMemory::AbsSkMemory():_ref(1) {

}
int AbsSkMemory::ref() {
    return _ref.fetch_add(1);
}
int AbsSkMemory::unRef() {
    LOGD("-- unRef ");
    return _ref.fetch_add(-1);
}

void AbsSkMemory::asConstant() {
    _constant = 1;
}
void AbsSkMemory::destroyData() {
    if(data){
        free(data);
        data = nullptr;
    }
}
//---------------------------------------------------------------------------------
SkMemory::SkMemory(const char *type, int len): AbsSkMemory(), _dType(type) {
    size = getUnitSize(type) * len;
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
            break;
    }
}
SkMemory::SkMemory(lua_State *L, int start, int tableCount, const char *t) : AbsSkMemory(){
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
            auto val = TO_NUMBER_8(L, -1);
            write_uint8_t(mem, index, val);
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
            auto v = read_uint8_t(mem, index);
            lua_pushnumber(L, v);
            return 1;
        }

        default:
            return luaL_error(L, "wrong data type = %s", mem->_dType);
    }
}
const char * SkMemory::toString() {
    SB::StringBuilder ss;
    switch (_dType[0]) {
        case 'f':
            Printer::printArray((float*)data, size / 4, ss);
            break;
        case 'd':
            Printer::printArray((uint32_t*)data, size / 4,ss );
            break;
        case 'w':
            Printer::printArray((uint16_t*)data, size / 2, ss);
            break;
        case 'b':
            Printer::printArray((uint8_t*)data, size, ss);
            break;
        default:
            return nullptr;
    }
    std::string sr;
    ss.toString(sr);
    const char *result = sr.c_str();
   /* auto i = SkUTF::CountUTF8(result, sr.length());
    LOGD("%s :  utf8 count = %d", result, i);*/
    return result;
}
//-----------------------------------------------------------------------
inline int SkMemory::getUnitSize(const char *t) {
    switch (t[0]) {
        case 'f':
        case 'd':
            return 4;
        case 'w':
            return 2;
        case 'b':
            return 1;

        default:
            return 0;
    }
}
inline int SkMemory::getTotalBytes(lua_State *L, int tableCount, const char *t){
    int minSize = getUnitSize(t);
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
    switch (_dType[0]) {
        case 'f':
        case 'd':
            return size / 4;
        case 'w':
            return size / 2;
        case 'b':
            return size;
    }
    return 0;
}

//------------------------ SkMemoryFFFI ----------------------
SkMemoryFFFUI::SkMemoryFFFUI(lua_State *L, int tableCount) : AbsSkMemory(){
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
const char* SkMemoryFFFUI::toString() {
    uint32_t * addr_ui = static_cast<uint32_t *>(data);
    float * addr_f = static_cast<float *>(data);

    std::ostringstream ss;
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
    return ss.str().c_str();
}
int SkMemoryFFFUI::getLength() {
    return size / 4;
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
SkMemoryMatrix::SkMemoryMatrix(lua_State *L, const char *type) {
    int tableCount = lua_gettop(L);
    count = tableCount;
    array = new SkMemory*[tableCount];
    for (int i = 0; i < tableCount; ++i) {
        array[i] = new SkMemory(L, i, 1, type);
        //LOGD("array[%d]: \n %s", i, array[i]->toString());//TODO乱码
    }
}
SkMemoryMatrix::SkMemoryMatrix(const char *type, int rowCount, int columnCount) {
    count = rowCount;
    array = new SkMemory*[rowCount];
    for (int i = 0; i < rowCount; ++i) {
        array[i] = new SkMemory(type, columnCount);
    }
}
void SkMemoryMatrix::destroyData() {
    if(array){
        delete[](array);
        array = nullptr;
        count = 0;
    }
}
bool SkMemoryMatrix::isValid() {
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
int SkMemoryMatrix::getRowCount() {
    return count;
}
int SkMemoryMatrix::getColumnCount() {
    return array ?array[0]->getLength() : 0;
}
const char* SkMemoryMatrix::toString() {
    std::ostringstream ss;
    ss << "{";
    for (int i = 0; i < count; ++i) {
        ss << array[i]->toString();
        LOGD("SkMemoryMatrix: %s", ss.str().c_str());
        if(i != count - 1){
            ss << ",";
        }
    }
    ss << "}";
    auto stdStr = ss.str();
    auto result = stdStr.c_str();
    return result;
}
