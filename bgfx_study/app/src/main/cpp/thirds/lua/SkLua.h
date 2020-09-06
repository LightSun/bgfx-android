/*
 * Copyright 2013 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkLua_DEFINED
#define SkLua_DEFINED

#include <stdint.h>

#include "bx/string.h"
#include "SkScalar.h"
#include "bgfx_wrapper.h"

struct lua_State;

extern "C" int luaopen_bgfx_lua(lua_State* L);
extern "C" int luaopen_bx_lua(lua_State* L);
extern "C" int luaopen_hmem_lua(lua_State* L);

class SkLua {
public:
    static void Load(lua_State*);

    SkLua(const char termCode[] = nullptr); // creates a new L, will close it
    SkLua(lua_State*);                      // uses L, will not close it
    ~SkLua();

    lua_State* get() const { return fL; }
    lua_State* operator*() const { return fL; }
    lua_State* operator->() const { return fL; }

    bool runCode(const char code[]);
    bool runCode(const void* code, size_t size);

    void pushBool(bool, const char tableKey[] = nullptr);
    void pushString(const char[], const char tableKey[] = nullptr);
    void pushString(const char[], size_t len, const char tableKey[] = nullptr);
    void pushString(const bx::StringView&, const char tableKey[] = nullptr);
    void pushArrayU16(const uint16_t[], int count, const char tableKey[] = nullptr);
    void pushArrayScalar(const SkScalar[], int count, const char key[] = nullptr);
    void pushU32(uint32_t, const char tableKey[] = nullptr);
    void pushScalar(SkScalar, const char tableKey[] = nullptr);

private:
    lua_State*  fL;
    bx::StringView    fTermCode;
    bool        fWeOwnL;
};

#endif
