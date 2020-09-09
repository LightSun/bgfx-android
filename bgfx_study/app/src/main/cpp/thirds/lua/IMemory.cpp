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

        default:
            return 0;
    }
}
void MemoryUtils::toString(SB::StringBuilder &sb, char t, void *data, int totalIndex) {
    uint8_t * addr = static_cast<uint8_t *>(data);
    addr += totalIndex;
    switch (t) {
        case 'f': {
            sb << *(float*)addr;
        }break;

        case 'd':{
            sb << *(uint32_t *)addr;
        }
        break;

        case 'w':{
            sb << *(uint16_t *)addr;
        }
        break;

        case 'b':{
            sb << *(uint8_t *)addr;
        }
        break;

        case 's':{
            sb << *(short *)addr;
        }
        break;
        case 'i':{
            sb << *(int *)addr;
        }
        break;
        case 'c':{
            sb << *(char *)addr;
        }
        break;

        default:
            LOGE("wrong type = %c", t);
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

        case 's':{
            *addr = static_cast<short>(lua_tointeger(L, -1));
        }break;
        case 'i':{
            *addr = static_cast<int >(lua_tointeger(L, -1));
        }break;
        case 'c':{
            *addr = static_cast<char>(lua_tointeger(L, -1));
        }break;

        default:
            luaL_error(L, "unknown type of %c", t);
            return -1;
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

        case 's':{
            lua_pushnumber(L, *(short *)addr);
        }break;
        case 'i':{
            lua_pushnumber(L, *(int *)addr);
        }break;
        case 'c':{
            lua_pushnumber(L, *(char *)addr);
        }break;

        default:
            luaL_error(L, "unknown type of %c", t);
            return -1;
    }
    return 0;
}
void MemoryUtils::init(char t, void *data, int totalIndex) {
    uint8_t * addr = static_cast<uint8_t *>(data);
    addr += totalIndex;
    switch (t) {
        case 'f': {
            *(float*)addr = 0;
        }
            break;
        case 'd':{
            *(uint32_t *)addr = 0;
        }break;
        case 'w':{
            *(uint16_t *)addr = 0;
        }break;
        case 'b':{
            *addr = 0;
        }break;

        case 's':{
            *(short *)addr = 0;
        }break;
        case 'i':{
            *(int *)addr = 0;
        }break;
        case 'c':{
            *(char*)addr = 0;
        }break;
        default:
            LOGW("unknown type data to init. %c", t);
    }
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