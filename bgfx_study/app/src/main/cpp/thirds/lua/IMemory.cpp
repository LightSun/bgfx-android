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
int MemoryUtils::read(lua_State *L, char t, void *data, int totalIndex) {
    uint8_t * addr = static_cast<uint8_t *>(data);
    addr += totalIndex;
    switch (t) {
        case 'f': {
            *(float*)addr =static_cast<float>(lua_tonumber(L, -1));
        }
            break;
        case 'd':{
            *(uint32_t*)addr = static_cast<uint32_t >(lua_tointeger(L, -1));
        }break;
        case 'w':{
            *(uint16_t*)addr = static_cast<uint16_t >(lua_tointeger(L, -1));
        }break;
        case 'b':{
            *addr = static_cast<uint8_t >(lua_tointeger(L, -1));
        }break;

        default:
            return luaL_error(L, "unknown type of %c", t);
    }
    return 0;
}
int MemoryUtils::write(lua_State *L, char t, void *data, int totalIndex) {
    uint8_t * addr = static_cast<uint8_t *>(data);
    addr += totalIndex;
    switch (t) {
        case 'f': {
            lua_pushnumber(L, *(float*)addr);
        }
            break;
        case 'd':{
            lua_pushnumber(L, *(uint32_t *)addr);
        }break;
        case 'w':{
            lua_pushnumber(L, *(uint16_t *)addr);
        }break;
        case 'b':{
            lua_pushnumber(L, *addr);
        }break;

        default:
            return luaL_error(L, "unknown type of %c", t);
    }
    return 0;
}
//-------------- memory allocator -------------------------
MemoryAllocator::MemoryAllocator():f(nullptr), u8(nullptr), u16(nullptr), u32(nullptr) {
}
MemoryAllocator::~MemoryAllocator() {
    releaseAll();
}
void MemoryAllocator::releaseAll() {
    FREE_POINTER(f)
    FREE_POINTER(u8)
    FREE_POINTER(u16)
    FREE_POINTER(u32)
}
void* MemoryAllocator::require(char type) {
    switch (type) {
        case 'f':
            if(!f){
                f = static_cast<float *>(malloc(sizeof(float)));
            }
            return f;
        case 'd':
            if(!u32){
                u32 = static_cast<uint32_t *>(malloc(sizeof(uint32_t)));
            }
            return u32;
        case 'w':
            if(!u16){
                u16 = static_cast<uint16_t *>(malloc(sizeof(uint16_t)));
            }
            return u16;
        case 'b':
            if(!u8){
                u8 = static_cast<uint8_t *>(malloc(sizeof(uint8_t)));
            }
            return u8;

        default:
            LOGW("unknown type : %c ", type);
            return nullptr;
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