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
#include "Gradient.hpp"

static int Canvas_moveTo(lua_State *L) {
    //(float x,float y)
    if(lua_gettop(L) != 2){
        return luaL_error(L, "wrong arguments. expect (float x,float y)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
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
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
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
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
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
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
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
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
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
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
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
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
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
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
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
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
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
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
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
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->ellipse(TO_FLOAT(L,1), TO_FLOAT(L,2), TO_FLOAT(L,3), TO_FLOAT(L,4));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_fill(lua_State *L) {
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->fill();
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_stroke(lua_State *L) {
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->stroke();
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_clearColor(lua_State *L) {
    if(lua_gettop(L) != 1){
        return luaL_error(L, "wrong arguments. expect (Color c)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    //auto c = lua_tostring(L, 1);
    NanoCanvas::Color* color = LuaUtils::get_ref<NanoCanvas::Color>(L, -1);
    canvas->clearColor(*color);
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_fillColor(lua_State *L) {
    if(lua_gettop(L) != 1){
        return luaL_error(L, "wrong arguments. expect (Color c)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    //auto c = lua_tostring(L, 1);
    NanoCanvas::Color* color = LuaUtils::get_ref<NanoCanvas::Color>(L, -1);
    canvas->fillColor(*color);
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_strokeColor(lua_State *L) {
    if(lua_gettop(L) != 1){
        return luaL_error(L, "wrong arguments. expect (Color c)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    //auto c = lua_tostring(L, 1);
    NanoCanvas::Color* color = LuaUtils::get_ref<NanoCanvas::Color>(L, -1);
    canvas->strokeColor(*color);
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_fillGradient(lua_State *L) {
    if(lua_gettop(L) != 1){
        return luaL_error(L, "wrong arguments. expect (Gradient c)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    //auto c = lua_tostring(L, 1);
    NanoCanvas::Gradient* color = LuaUtils::get_ref<NanoCanvas::Gradient>(L, -1);
    canvas->fillGradient(*color);
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_strokeGradient(lua_State *L) {
    if(lua_gettop(L) != 1){
        return luaL_error(L, "wrong arguments. expect (Gradient c)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    //auto c = lua_tostring(L, 1);
    NanoCanvas::Gradient* color = LuaUtils::get_ref<NanoCanvas::Gradient>(L, -1);
    canvas->strokeGradient(*color);
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
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    if(top == 3){
        canvas->fillText(lua_tostring(L, 1), TO_FLOAT(L, 2), TO_FLOAT(L, 3));
    } else{
        canvas->fillText(lua_tostring(L, 1), TO_FLOAT(L, 2), TO_FLOAT(L, 3), TO_FLOAT(L, 4));
    }
    lua_pushvalue(L, canvasIndex);
    return 1;
}
//Canvas& lineCap(LineCap cap);
static int Canvas_lineCap(lua_State *L) {
    //BUTT,ROUND,SQUARE
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    auto str = lua_tostring(L, 1);
    if(strcmp("ROUND", str) == 0){
        canvas->lineCap(NanoCanvas::Canvas::LineCap::ROUND);
    } else if(strcmp("BUTT", str) == 0){
        canvas->lineCap(NanoCanvas::Canvas::LineCap::BUTT);
    }else if(strcmp("SQUARE", str) == 0){
        canvas->lineCap(NanoCanvas::Canvas::LineCap::SQUARE);
    }else{
        return luaL_error(L, "wrong line cap. expect is 'BUTT/ROUND/SQUARE'");
    }
    lua_pushvalue(L, canvasIndex);
    return 1;
}
//Canvas& lineJoin(LineJoin cap);
static int Canvas_lineJoin(lua_State *L) {
    //BEVEL,ROUND,MITER
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    auto str = lua_tostring(L, 1);
    if(strcmp("ROUND", str) == 0){
        canvas->lineJoin(NanoCanvas::Canvas::LineJoin::ROUND);
    } else if(strcmp("MITER", str) == 0){
        canvas->lineJoin(NanoCanvas::Canvas::LineJoin::MITER);
    }else if(strcmp("BEVEL", str) == 0){
        canvas->lineJoin(NanoCanvas::Canvas::LineJoin::BEVEL);
    }else{
        return luaL_error(L, "wrong line join. expect is 'BEVEL/ROUND/MITER'");
    }
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_lineWidth(lua_State *L) {
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->lineWidth(TO_FLOAT(L, 1));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_miterLimit(lua_State *L) {
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->miterLimit(TO_FLOAT(L, 1));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_globalAlpha(lua_State *L) {
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->globalAlpha(TO_FLOAT(L, 1));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
//TODO font/fontSize...etc.
//TODO drawImage
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
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, -1);
    nvgDelete(canvas->nvgContext());
    delete canvas;
    return 0;
}
static int Color_gc(lua_State *L) {
    NanoCanvas::Color* color = LuaUtils::get_ref<NanoCanvas::Color>(L, -1);
    delete color;
    return 0;
}
static int Gradient_gc(lua_State *L) {
    NanoCanvas::Gradient* g = LuaUtils::get_ref<NanoCanvas::Gradient>(L, -1);
    delete g;
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
    const static luaL_Reg Gradient_Methods[] = {
            {"__gc",              Gradient_gc},
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
static int newLinearGradient(lua_State *L) {
    //(float x0, float y0, float x1, float y1,  const Color& scolor , const Color& ecolor)
    if(lua_gettop(L) < 6){
        return luaL_error(L, "wrong arguments for newLinearGradient. expect params are (float x0, float y0, float x1, float y1,"
                             " const Color& scolor , const Color& ecolor)");
    }
    auto scolor = LuaUtils::get_ref<NanoCanvas::Color>(L, 5);
    auto ecolor = LuaUtils::get_ref<NanoCanvas::Color>(L, 6);
    auto gradient = NanoCanvas::Canvas::createLinearGradient(TO_FLOAT(L, 1), TO_FLOAT(L, 2),
                                                             TO_FLOAT(L, 3), TO_FLOAT(L, 4),
                                                             scolor, ecolor);
    LuaUtils::push_ptr(L, gradient);
    return 1;
}
static int newRadialGradient(lua_State *L) {
    //(float x0, float y0, float x1, float y1,  const Color& scolor , const Color& ecolor)
    if(lua_gettop(L) < 6){
        return luaL_error(L, "wrong arguments for newLinearGradient. expect params are (float x0, float y0, float x1, float y1,"
                             " const Color& scolor , const Color& ecolor)");
    }
    auto scolor = LuaUtils::get_ref<NanoCanvas::Color>(L, 5);
    auto ecolor = LuaUtils::get_ref<NanoCanvas::Color>(L, 6);
    auto gradient = NanoCanvas::Canvas::createRadialGradient(TO_FLOAT(L, 1), TO_FLOAT(L, 2),
                                                             TO_FLOAT(L, 3), TO_FLOAT(L, 4),
                                                             scolor, ecolor);
    LuaUtils::push_ptr(L, gradient);
    return 1;
}
static int newBoxGradient(lua_State *L) {
    //(float x0, float y0, float x1, float y1,  const Color& scolor , const Color& ecolor)
    if(lua_gettop(L) < 8){
        return luaL_error(L, "wrong arguments for newLinearGradient. expect params are (float x0, float y0, float x1, float y1,"
                             " const Color& scolor , const Color& ecolor)");
    }
    auto icol = LuaUtils::get_ref<NanoCanvas::Color>(L, 7);
    auto ocol = LuaUtils::get_ref<NanoCanvas::Color>(L, 8);
    auto gradient = NanoCanvas::Canvas::createBoxGradient(TO_FLOAT(L, 1), TO_FLOAT(L, 2),
                                                             TO_FLOAT(L, 3), TO_FLOAT(L, 4),
                                                             TO_FLOAT(L, 5), TO_FLOAT(L, 6),
                                                          icol, ocol);
    LuaUtils::push_ptr(L, gradient);
    return 1;
}
static int newImageGradient(lua_State *L) {
    //(float x0, float y0, float x1, float y1,  const Color& scolor , const Color& ecolor)
    if(lua_gettop(L) < 6){
        return luaL_error(L, "wrong arguments for newLinearGradient. expect params are (float x0, float y0, float x1, float y1,"
                             " const Color& scolor , const Color& ecolor)");
    }
    auto scolor = LuaUtils::get_ref<NanoCanvas::Color>(L, 5);
    auto ecolor = LuaUtils::get_ref<NanoCanvas::Color>(L, 6);
    //TODO
    /*auto gradient = NanoCanvas::Canvas::createImageGradient(TO_FLOAT(L, 1), TO_FLOAT(L, 2),
                                                             TO_FLOAT(L, 3), TO_FLOAT(L, 4),
                                                             scolor, ecolor);
    LuaUtils::push_ptr(L, gradient);*/
    return 1;
}

static const luaL_Reg mem_funcs[] = {
        {"newCanvas", newCanvas},
        {"newColor",  newColor},
        {"newLinearGradient",  newLinearGradient},
        {"newRadialGradient",  newRadialGradient},
        {"newBoxGradient",     newBoxGradient},
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
    REG_CLASS(L, NanoCanvas::Gradient);
}