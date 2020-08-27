//
// Created by Administrator on 2020/8/27 0027.
//
#include "SkMemory.h"
#include "common.h"

#define copy_data_f(dType, step) \
char * addr = static_cast<char *>(data); \
for (int i = 0; i < tableCount; ++i) { \
    idx = 1; \
    type = lua_rawgeti(L, i + 1, idx); \
    for (; type != LUA_TNIL ;) { \
        *addr = static_cast<dType>(lua_tonumber(L, -1)); \
        addr += step; \
        lua_pop(L, 1); \
        ++idx; \
        type = lua_rawgeti(L, i + 1, idx); \
    } \
}

#define copy_data_i(dType, step) \
char * addr = static_cast<char *>(data); \
for (int i = 0; i < tableCount; ++i) { \
    idx = 1; \
    type = lua_rawgeti(L, i + 1, idx); \
    for (; type != LUA_TNIL ;) { \
        *addr = static_cast<dType>(lua_tointeger(L, -1)); \
        addr += step; \
        lua_pop(L, 1); \
        ++idx; \
        type = lua_rawgeti(L, i + 1, idx); \
    } \
}

AbsSkMemory::~AbsSkMemory() {
    destroyData();
}
AbsSkMemory::AbsSkMemory():_ref(1) {

}
int AbsSkMemory::ref() {
    return _ref.fetch_add(1);
}
int AbsSkMemory::unRef() {
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
SkMemory::SkMemory(lua_State *L, int tableCount, const char *t) : AbsSkMemory(){
    _dType = t;
    size = getTotalBytes(L, tableCount, t);
    SkASSERT(size > 0);
    data = malloc(size);
    int type;
    int idx;
    switch (t[0]) {
        case 'f': {
            copy_data_f(float, 4);
        }
            break;
        case 'd': {
            copy_data_i(uint32_t, 4);
        }
            break;
        case 'w': {
            copy_data_i(uint16_t, 2);
        }
            break;
        case 'b':{
            copy_data_i(uint8_t , 2);
        }
            break;
    }
    lua_pop(L, 1);
}

float SkMemory::readFloat(size_t index) {
    float* addr = (float*)data;
    addr += index;
    return *addr;
}
uint8_t SkMemory::readUByte(size_t index) {
    uint8_t* addr = (uint8_t*)data;
    addr += index;
    return *addr;
}
uint16_t SkMemory::readUShort(size_t index) {
    uint16_t* addr = (uint16_t*)data;
    addr += index;
    return *addr;
}
uint32_t SkMemory::readUInt32(size_t index) {
    uint32_t* addr = (uint32_t*)data;
    addr += index;
    return *addr;
}
void SkMemory::writeFloat(size_t index, float val) {
    float* addr = (float*)data;
    addr += index;
    *addr = val;
}
void SkMemory::writeUByte(size_t index, uint8_t val){
    uint8_t * addr = (uint8_t*)data;
    addr += index;
    *addr = val;
}
void SkMemory::writeUShort(size_t index, uint16_t val) {
    uint16_t * addr = (uint16_t*)data;
    addr += index;
    *addr = val;
}
void SkMemory::writeUInt32(size_t index, uint32_t val) {
    uint32_t * addr = (uint32_t*)data;
    addr += index;
    *addr = val;
}

//-----------------------------------------------------------------------
inline uint32_t SkMemory::getTotalBytes(lua_State *L, int tableCount, const char *t){
    uint8_t minSize;
    switch (t[0]) {
        case 'f':
        case 'd':
            minSize = 4;
            break;
        case 'w':
            minSize = 2;
            break;
        case 'b':
            minSize = 1;
            break;

        default:
            luaL_error(L, "wrong type of memory-unit: %s", t);
            return 0;
    }
    uint32_t totalSize = 0 ;
    for (int i = 0; i < tableCount; ++i) {
        size_t len = lua_rawlen(L, i + 1);
        totalSize += len * minSize;
    }
    return totalSize;
}

//------------------------ SkMemoryFFFI ----------------------
SkMemoryFFFUI::SkMemoryFFFUI(lua_State *L, int tableCount) : AbsSkMemory(){
    size = 4 * 4 * tableCount;
    SkASSERT(size > 0);
    data = malloc(size);

    char * addr = static_cast<char *>(data);
    int type;
    for (int i = 0; i < tableCount; ++i) {
        for (int idx = 0; idx < 3; ++idx) {
            type = lua_rawgeti(L, i + 1, idx + 1);
            if(type != LUA_TNUMBER){
                luaL_error(L, "create SkMemoryFFFUI failed. for error data type = %d", type);
                goto out;
            }
            *addr = static_cast<float >(lua_tonumber(L, -1));
            addr += 4;
            lua_pop(L, 1);
        }
        type = lua_rawgeti(L, i + 1, 4);
        if(type != LUA_TNUMBER){
            luaL_error(L, "create SkMemoryFFFUI failed. for error data type = %d", type);
            goto out;
        }
        *addr = static_cast<uint32_t>(lua_tointeger(L, -1));
        addr += 4;
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    out:
        unRef();
}