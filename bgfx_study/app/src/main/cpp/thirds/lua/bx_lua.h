//
// Created by Administrator on 2020/9/11 0011.
//

#ifndef BGFX_STUDY_BX_LUA_H
#define BGFX_STUDY_BX_LUA_H

struct lua_State{

};

extern "C" int luaopen_bx_lua(lua_State *L);

class BxLua{
public:
    static void registers(lua_State *L);
};

#endif //BGFX_STUDY_BX_LUA_H
