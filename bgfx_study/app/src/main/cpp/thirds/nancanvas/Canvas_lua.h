//
// Created by Administrator on 2020/11/3 0003.
//

#ifndef BGFX_STUDY_CANVAS_LUA_H
#define BGFX_STUDY_CANVAS_LUA_H

struct lua_State;

extern "C" int luaopen_canvas_lua(lua_State *L);

class CanvasLua{
public:
    static void registers(lua_State *L);
}

#endif //BGFX_STUDY_CANVAS_LUA_H
