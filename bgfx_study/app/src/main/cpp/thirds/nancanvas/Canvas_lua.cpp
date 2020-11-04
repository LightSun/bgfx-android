//
// Created by Administrator on 2020/11/3 0003.
//

#include "../lua/lua.hpp"
#include "../lua/LuaUtils.h"
#include "../lua/LuaUtils.h"
#include "Canvas_lua.h"
#include "Canvas.h"
#include "nanovg/nanovg_bgfx.h"
#include "Color.hpp"

static int Canvas_moveTo(lua_State *L) {
    //(float x,float y)
    if(lua_gettop(L) != 2){
        return luaL_error(L, "wrong arguments. expect (float x,float y)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref(L, canvasIndex);
    canvas->moveTo(TO_FLOAT(L,1), TO_FLOAT(L,2));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_lineTo(lua_State *L) {
    //lineTo(float x,float y)
    if(lua_gettop(L) != 2){
        return luaL_error(L, "wrong arguments. expect (float x,float y)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref(L, canvasIndex);
    canvas->lineTo(TO_FLOAT(L,1), TO_FLOAT(L,2));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_arcTo(lua_State *L) {
    //arcTo(float x1,float y1,float x2,float y2,float r)
    if(lua_gettop(L) != 5){
        return luaL_error(L, "wrong arguments. expect (float x1,float y1,float x2,float y2,float r)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref(L, canvasIndex);
    canvas->arcTo(TO_FLOAT(L,1), TO_FLOAT(L,2), TO_FLOAT(L,3), TO_FLOAT(L,4), TO_FLOAT(L,5));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_arc(lua_State *L) {
    //arc(float x,float y,float r,
    //                    float sAngle,float eAngle,bool counterclockwise = false)
    if(lua_gettop(L) < 5){
        return luaL_error(L, "wrong arguments. expect (float x1,float y1,float x2,float y2,float r)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref(L, canvasIndex);
    if(lua_gettop(L) == 5){
        canvas->arc(TO_FLOAT(L,1), TO_FLOAT(L,2), TO_FLOAT(L,3), TO_FLOAT(L,4), TO_FLOAT(L,5));
    } else{
        canvas->arc(TO_FLOAT(L,1), TO_FLOAT(L,2), TO_FLOAT(L,3), TO_FLOAT(L,4), TO_FLOAT(L,5),
                lua_toboolean(L, 6) == 1);
    }
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_closePath(lua_State *L) {
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref(L, canvasIndex);
    canvas->closePath();
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_rect(lua_State *L) {
    //rect(float x,float y,float w,float h)
    if(lua_gettop(L) != 4){
        return luaL_error(L, "wrong arguments. expect (float x,float y,float w,float h)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref(L, canvasIndex);
    canvas->rect(TO_FLOAT(L,1), TO_FLOAT(L,2), TO_FLOAT(L,3), TO_FLOAT(L,4));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_fillRect(lua_State *L) {
    //fillRect(float x,float y,float w,float h)
    if(lua_gettop(L) != 4){
        return luaL_error(L, "wrong arguments. expect (float x,float y,float w,float h)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref(L, canvasIndex);
    canvas->fillRect(TO_FLOAT(L,1), TO_FLOAT(L,2), TO_FLOAT(L,3), TO_FLOAT(L,4));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_strokeRect(lua_State *L) {
    //fillRect(float x,float y,float w,float h)
    if(lua_gettop(L) != 4){
        return luaL_error(L, "wrong arguments. expect (float x,float y,float w,float h)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref(L, canvasIndex);
    canvas->strokeRect(TO_FLOAT(L,1), TO_FLOAT(L,2), TO_FLOAT(L,3), TO_FLOAT(L,4));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_roundedRect(lua_State *L) {
    //roundedRect(float x,float y,float w,float h, float r)
    if(lua_gettop(L) != 5){
        return luaL_error(L, "wrong arguments. expect (float x,float y,float w,float h, float r)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref(L, canvasIndex);
    canvas->roundedRect(TO_FLOAT(L,1), TO_FLOAT(L,2), TO_FLOAT(L,3), TO_FLOAT(L,4), TO_FLOAT(L,5));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_circle(lua_State *L) {
    //circle(float cx ,float cy , float r)
    if(lua_gettop(L) != 3){
        return luaL_error(L, "wrong arguments. expect (float cx ,float cy , float r)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref(L, canvasIndex);
    canvas->circle(TO_FLOAT(L,1), TO_FLOAT(L,2), TO_FLOAT(L,3));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
//ellipse(float cx, float cy, float rx, float ry)
static int Canvas_ellipse(lua_State *L) {
    if(lua_gettop(L) != 4){
        return luaL_error(L, "wrong arguments. expect (float cx, float cy, float rx, float ry)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref(L, canvasIndex);
    canvas->ellipse(TO_FLOAT(L,1), TO_FLOAT(L,2), TO_FLOAT(L,3), TO_FLOAT(L,4));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_fill(lua_State *L) {
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref(L, canvasIndex);
    canvas->fill();
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_stroke(lua_State *L) {
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref(L, canvasIndex);
    canvas->stroke();
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_clearColor(lua_State *L) {
    if(lua_gettop(L) != 1){
        return luaL_error(L, "wrong arguments. expect (Color c)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref(L, canvasIndex);
    //auto c = lua_tostring(L, 1);
    NanoCanvas::Color* color = LuaUtils::get_ref(L, -1);
    canvas->clearColor(*color);
    lua_pushvalue(L, canvasIndex);
    return 1;
}
//fillText(const char *text, float x, float y, float rowWidth)
static int Canvas_fillText(lua_State *L) {
    int top = lua_gettop(L);
    if(top < 3){
        return luaL_error(L, "wrong arguments. expect (const char *text, float x, float y, [float rowWidth])");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref(L, canvasIndex);
    if(top == 3){
        canvas->fillText(lua_tostring(L, 1), TO_FLOAT(L, 2), TO_FLOAT(L, 3));
    } else{
        canvas->fillText(lua_tostring(L, 1), TO_FLOAT(L, 2), TO_FLOAT(L, 3), TO_FLOAT(L, 4));
    }
    lua_pushvalue(L, canvasIndex);
    return 1;
}
namespace sNanoCanvas{
    const static luaL_Reg Canvas_Methods[] = {
            {"moveTo",           Canvas_moveTo},
            {"lineTo",           Canvas_lineTo},
            {"arcTo",            Canvas_arcTo},
            {"arc",              Canvas_arc},
            {"closePath",        Canvas_closePath},
            {NULL, NULL}
    };
}
DEF__INDEX(Canvas, NanoCanvas::Canvas)

static int Canvas_gc(lua_State *L) {
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref(L, -1);
    nvgDelete(canvas->nvgContext());
    delete canvas;
    return 0;
}
static int Color_gc(lua_State *L) {
    NanoCanvas::Color* color = LuaUtils::get_ref(L, -1);
    delete color;
    return 0;
}

namespace gNanoCanvas{
    const static luaL_Reg Canvas_Methods[] = {
            {"__index",           Canvas_index},
            {"__gc",              Canvas_gc},
            {NULL, NULL}
    };
    const static luaL_Reg Color_Methods[] = {
            {"__gc",              Color_gc},
            {NULL, NULL}
    };
}
//---------------- lib methods --------------------------

static int newCanvas(lua_State *L) {
    //(viewId, width, height, [scaleRatio])
    auto viewId = lua_tointeger(L, 1);
    NanoCanvas::Canvas* canvas;
    switch (lua_gettop(L)){
        case 4: {
            NVGcontext *context = nvgCreate(1, viewId);
            canvas = new NanoCanvas::Canvas(context,
                                            lua_tointeger(L,2), lua_tointeger(L, 3), (float)lua_tonumber(L, 4));
        }
            break;
        case 3:{
            NVGcontext* context = nvgCreate(1, viewId);
            canvas = new NanoCanvas::Canvas(context,
                                            lua_tointeger(L,2), lua_tointeger(L, 3));
        }
            break;
        default:
            return luaL_error(L, "wrong arguments. expect is (viewId, width, height, [scaleRatio])");
    }
    bgfx::setViewMode(static_cast<bgfx::ViewId>(viewId), bgfx::ViewMode::Sequential);
    LuaUtils::push_ptr(L, canvas);
    return 1;
}
static int newColor(lua_State *L) {
    NanoCanvas::Color * c = nullptr;
    auto t = lua_type(L, -1);
    if(t == LUA_TSTRING){
        c = new NanoCanvas::Color();
        c->set(lua_tostring(L, -1));
    } else if(t == LUA_TNUMBER){
        c = new NanoCanvas::Color();
        c->set(lua_tointeger(L, -1));
    } else{
        return luaL_error(L, "color only support. string or int");
    }
    LuaUtils::push_ptr(L, c);
    return 1;
}

static const luaL_Reg mem_funcs[] = {
        {"newCanvas", newCanvas},
        {"newColor",  newColor},
        {nullptr,     nullptr}
};
extern "C" int luaopen_hmem_lua(lua_State *L) {
    luaL_newlib(L, mem_funcs);
    return 1;
}

DEF_MTNAME(NanoCanvas::Canvas)
void CanvasLua::registers(lua_State *L) {
    REG_CLASS(L, NanoCanvas::Canvas);
    REG_CLASS(L, NanoCanvas::Color);
}