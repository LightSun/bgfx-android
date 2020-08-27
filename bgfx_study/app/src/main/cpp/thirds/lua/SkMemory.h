//
// Created by Administrator on 2020/8/27 0027.
//

#ifndef BGFX_STUDY_SKMEMORY_H
#define BGFX_STUDY_SKMEMORY_H

#include <stdint.h>
#include <atomic>
#include "lua.hpp"

class SkMemory{

public:

    ~SkMemory();
    /**
     * one memory contains can multi table.
     * @param L
     * @param tableCount
     * @param minUnitSize
     */
    SkMemory(lua_State* L, int tableCount, const char* type);

    int ref();

    int unRef();

    void asConstant();

    void destroyData();

    //------ index start from 0 --------
    uint32_t readUInt32(size_t index);
    uint8_t readUByte(size_t index);
    uint16_t readUShort(size_t index);
    float readFloat(size_t index);

    void writeUInt32(size_t index, uint32_t val);
    void writeUByte(size_t index, uint8_t val);
    void writeUShort(size_t index, uint16_t val);
    void writeFloat(size_t index, float val);

public:
    void *data;
    size_t size;
    uint8_t _constant; // 1 is constant. 0 not.
    std::atomic_int _ref;

private:
    const char * _dType;
    uint32_t getTotalBytes(lua_State *L, int tableCount, const char *t);
};

/*typedef struct PosColorVertex {
    float m_x;
    float m_y;
    float m_z;
    uint32_t m_abgr;
} PosColorVertex;*/

#endif //BGFX_STUDY_SKMEMORY_H
