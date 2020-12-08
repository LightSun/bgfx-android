//
// Created by Administrator on 2020/11/3 0003.
//

#include "Text.h"
#include "Color.h"
#include "Gradient.h"
#include "Image.h"
#include "Canvas.h"
#include "../lua/lua.hpp"
#include "../lua/LuaUtils.h"
#include "../lua/LuaUtils.h"
#include "../lua/SkMemory.h"
#include "Canvas_lua.h"
#include "nanovg/nanovg_bgfx.h"
#include "utils/Array.h"
#include <log.h>

#define CANVAS_M0(name)\
static int Canvas_##name(lua_State *L) {\
    auto canvasIndex = lua_upvalueindex(1);\
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);\
    canvas->name();\
    lua_pushvalue(L, canvasIndex);\
    return 1;\
}
#define CANVAS_M0_NO_RETURN(name)\
static int Canvas_##name(lua_State *L) {\
    auto canvasIndex = lua_upvalueindex(1);\
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);\
    canvas->name();\
    return 0;\
}

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
static int Canvas_quadTo(lua_State *L) {
    //(float cpx,float cpy,float x,float y)
    if(lua_gettop(L) < 4){
        return luaL_error(L, "wrong arguments. expect (float x1,float y1,float x2,float y2,float r)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->quadTo(TO_FLOAT(L,1), TO_FLOAT(L,2), TO_FLOAT(L,3), TO_FLOAT(L,4));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_bezierTo(lua_State *L) {
    //(float cpx,float cpy,float cpx2,float cpy2, float x,float y)
    if(lua_gettop(L) < 6){
        return luaL_error(L, "wrong arguments. expect (float x1,float y1,float x2,float y2,float r)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->bezierTo(TO_FLOAT(L,1), TO_FLOAT(L,2), TO_FLOAT(L,3),
            TO_FLOAT(L,4), TO_FLOAT(L,5), TO_FLOAT(L,6));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_arc(lua_State *L) {
    //arc(float x,float y,float r, float sAngle ,float eAngle, bool counterclockwise = false)
    if(lua_gettop(L) < 5){
        return luaL_error(L, "wrong arguments. expect (float x,float y,float r, float sAngle, float eAngle, [bool counterclockwise = false])");
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
static int Canvas_clearColor(lua_State *L) {
    if(lua_gettop(L) != 1){
        return luaL_error(L, "wrong arguments. expect (Color c)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    //auto c = lua_tostring(L, 1);
    h7::Color* color = LuaUtils::get_ref<h7::Color>(L, -1);
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
    h7::Color* color = LuaUtils::get_ref<h7::Color>(L, -1);
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
    h7::Color* color = LuaUtils::get_ref<h7::Color>(L, -1);
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
    auto str = lua_tointeger(L, 1);
    switch (str){
        case 1:
            canvas->lineCap(NanoCanvas::Canvas::LineCap::BUTT);
            break;
        case 2:
            canvas->lineCap(NanoCanvas::Canvas::LineCap::SQUARE);
            break;
        case 3:
            canvas->lineCap(NanoCanvas::Canvas::LineCap::ROUND);
            break;
        default:
            return luaL_error(L, "wrong arguments for canvas.lineCap()");
    }
    lua_pushvalue(L, canvasIndex);
    return 1;
}
//Canvas& lineJoin(LineJoin cap);
static int Canvas_lineJoin(lua_State *L) {
    //BEVEL,ROUND,MITER
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    auto str = lua_tointeger(L, 1);
    switch (str){
        case 3:
            canvas->lineJoin(NanoCanvas::Canvas::LineJoin::ROUND);
            break;
        case 4:
            canvas->lineJoin(NanoCanvas::Canvas::LineJoin::BEVEL);
            break;
        case 5:
            canvas->lineJoin(NanoCanvas::Canvas::LineJoin::MITER);
            break;
        default:
            return luaL_error(L, "wrong arguments for canvas.lineJoin()");
    }
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_strokeWidth(lua_State *L) {
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->strokeWidth(TO_FLOAT(L, 1));
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
static int Canvas_font(lua_State *L) {
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    NanoCanvas::Font* font = LuaUtils::get_ref<NanoCanvas::Font>(L, 1);
    canvas->font(*font);
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_fontSize(lua_State *L) {
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->fontSize(TO_FLOAT(L, 1));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_textAlign(lua_State *L) {
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->textAlign(TO_INT(L, 1), TO_INT(L, 2));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_textStyle(lua_State *L) {
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    NanoCanvas::TextStyle* ts = LuaUtils::get_ref<NanoCanvas::TextStyle>(L, 1);
    canvas->textStyle(*ts);
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_measureText(lua_State *L) {
    //(const char* text,float x,float y, [float rowWidth])
    int top = lua_gettop(L);
    if(top < 3){
        return luaL_error(L, "wrong arguments. expect (const char *text, float x, float y, [float rowWidth])");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    float w, h;
    if(top == 3){
        canvas->measureText(lua_tostring(L, 1), TO_FLOAT(L, 2), TO_FLOAT(L, 3), &w, &h);
    } else{
        canvas->measureText(lua_tostring(L, 1), TO_FLOAT(L, 2), TO_FLOAT(L, 3), &w, &h, TO_FLOAT(L, 4));
    }
    lua_pushnumber(L, w);
    lua_pushnumber(L, h);
    return 2;
}

static int Canvas_scale(lua_State *L) {
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->scale(TO_FLOAT(L, 1), TO_FLOAT(L, 2));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_rotate(lua_State *L) {
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->rotate(TO_FLOAT(L, 1));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_translate(lua_State *L) {
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->translate(TO_FLOAT(L, 1), TO_FLOAT(L, 2));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_transform(lua_State *L) {
    //(float a, float b, float c, float d, float e, float f)
    int top = lua_gettop(L);
    if(top < 6){
        return luaL_error(L, "wrong arguments. expect (float a, float b, float c, float d, float e, float f)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->transform(TO_FLOAT(L, 1), TO_FLOAT(L, 2), TO_FLOAT(L, 3),
                      TO_FLOAT(L, 4), TO_FLOAT(L, 5), TO_FLOAT(L, 6));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_setTransform(lua_State *L) {
    //float a, float b, float c, float d, float e, float f
    int top = lua_gettop(L);
    if(top < 6){
        return luaL_error(L, "wrong arguments. expect (float a, float b, float c, float d, float e, float f)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->setTransform(TO_FLOAT(L, 1), TO_FLOAT(L, 2), TO_FLOAT(L, 3),
                      TO_FLOAT(L, 4), TO_FLOAT(L, 5), TO_FLOAT(L, 6));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_beginFrame(lua_State *L) {
    //float a, float b, float c, float d, float e, float f
    int top = lua_gettop(L);
    if(top < 2){
        return luaL_error(L, "wrong arguments. expect (float a, float b, float c, float d, float e, float f)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->beginFrame(TO_INT(L, 1), TO_INT(L, 2));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_clip(lua_State *L) {
    //(float x, float y, float w, float h)
    int top = lua_gettop(L);
    if(top < 4){
        return luaL_error(L, "wrong arguments. expect (float x, float y, float w, float h)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->clip(TO_FLOAT(L, 1), TO_FLOAT(L, 2), TO_FLOAT(L, 3),
                 TO_FLOAT(L, 4));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_clipOut(lua_State *L) {
    //(float x, float y, float w, float h)
    int top = lua_gettop(L);
    if(top < 4){
        return luaL_error(L, "wrong arguments. expect (float x, float y, float w, float h)");
    }
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->clipOut(TO_FLOAT(L, 1), TO_FLOAT(L, 2), TO_FLOAT(L, 3),
                 TO_FLOAT(L, 4));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_pathWinding(lua_State *L) {
    //float a, float b, float c, float d, float e, float f
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->pathWinding(TO_INT(L, 1) == 1 ? NanoCanvas::Canvas::Winding::CCW : NanoCanvas::Canvas::Winding::CW);
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_setSize(lua_State *L) {
    //float a, float b, float c, float d, float e, float f
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->setSize(TO_INT(L, 1), TO_INT(L, 2));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_setPosition(lua_State *L) {
    //float a, float b, float c, float d, float e, float f
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->setPosition(TO_FLOAT(L, 1), TO_FLOAT(L, 2));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_offsetPosition(lua_State *L) {
    //float a, float b, float c, float d, float e, float f
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->offsetPosition(TO_FLOAT(L, 1), TO_FLOAT(L, 2));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_getPosition(lua_State *L) {
    //float a, float b, float c, float d, float e, float f
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    float f1 = 0;
    float f2 = 0;
    canvas->local2Global(f1, f2);
    lua_pushnumber(L, f1);
    lua_pushnumber(L, f2);
    return 1;
}
static int Canvas_setScaleRatio(lua_State *L) {
    //float a, float b, float c, float d, float e, float f
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    canvas->setScaleRatio(TO_FLOAT(L, 1));
    lua_pushvalue(L, canvasIndex);
    return 1;
}
static int Canvas_local2Global(lua_State *L) {
    //float a, float b, float c, float d, float e, float f
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    float f1 = TO_FLOAT(L, 1);
    float f2 = TO_FLOAT(L, 2);
    canvas->local2Global(f1, f2);
    lua_pushnumber(L, f1);
    lua_pushnumber(L, f2);
    return 1;
}
static int Canvas_global2Local(lua_State *L) {
    //float a, float b, float c, float d, float e, float f
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    float f1 = TO_FLOAT(L, 1);
    float f2 = TO_FLOAT(L, 2);
    canvas->global2Local(f1, f2);
    lua_pushnumber(L, f1);
    lua_pushnumber(L, f2);
    return 1;
}
static int Canvas_getContext(lua_State *L) {
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, lua_upvalueindex(1));
    auto context = canvas->nvgContext();
    LuaUtils::push_ptr(L, context);
    return 1;
}
static int Canvas_drawImage(lua_State *L) {
    //(Image& image,float x, float y,
    //    float width = NAN,float height = NAN,
    //    float sx = 0,float sy = 0,
    //    float swidth = NAN,float sheight = NAN)
    auto canvasIndex = lua_upvalueindex(1);
    NanoCanvas::Canvas* canvas = LuaUtils::get_ref<NanoCanvas::Canvas>(L, canvasIndex);
    NanoCanvas::Image* img = LuaUtils::get_ref<NanoCanvas::Image>(L, 1);
    switch(lua_gettop(L)){
        case 3:{
            canvas->drawImage(*img, TO_FLOAT(L,2), TO_FLOAT(L,3));
        }
            break;
        case 4:{
            canvas->drawImage(*img, TO_FLOAT(L,2), TO_FLOAT(L,3), TO_FLOAT(L,4));
        }
            break;
        case 5:{
            canvas->drawImage(*img, TO_FLOAT(L,2), TO_FLOAT(L,3), TO_FLOAT(L,4), TO_FLOAT(L,5));
        }
            break;
        case 6:{
            canvas->drawImage(*img, TO_FLOAT(L,2), TO_FLOAT(L,3), TO_FLOAT(L,4),
                    TO_FLOAT(L,5), TO_FLOAT(L,6));
        }
            break;
        case 7:{
            canvas->drawImage(*img, TO_FLOAT(L,2), TO_FLOAT(L,3), TO_FLOAT(L,4),
                              TO_FLOAT(L,5), TO_FLOAT(L,6), TO_FLOAT(L,7));
        }
            break;
        case 8:{
            canvas->drawImage(*img, TO_FLOAT(L,2), TO_FLOAT(L,3), TO_FLOAT(L,4),
                              TO_FLOAT(L,5), TO_FLOAT(L,6), TO_FLOAT(L,7), TO_FLOAT(L,8));
        }
            break;
        case 9:{
            canvas->drawImage(*img, TO_FLOAT(L,2), TO_FLOAT(L,3), TO_FLOAT(L,4),
                              TO_FLOAT(L,5), TO_FLOAT(L,6), TO_FLOAT(L,7),
                              TO_FLOAT(L,8), TO_FLOAT(L,9));
        }
            break;
            //(Image& image,float x, float y,float width = NAN,float height = NAN,float sx = 0,float sy = 0,float swidth = NAN,float sheight = NAN)
        default:
            return luaL_error(L, "wrong arguments for drawImage. expect (Image& image,float x, float y,"
                                 "[float width = NAN,float height = NAN,float sx = 0,float sy = 0,"
                                 "float swidth = NAN,float sheight = NAN] )");
    }
    lua_pushvalue(L, canvasIndex);
    return 1;
}
CANVAS_M0(resetTransform)
CANVAS_M0(cancelFrame)
CANVAS_M0(beginPath)
CANVAS_M0(resetClip)
CANVAS_M0(save)
CANVAS_M0(restore)
CANVAS_M0(reset)
CANVAS_M0(closePath)
CANVAS_M0(fill)
CANVAS_M0(stroke)
CANVAS_M0_NO_RETURN(endFrame)

namespace sNanoCanvas{
#define CANVAS_M_RAW(name)\
{#name,    Canvas_##name},
    const static luaL_Reg Canvas_Methods[] = {
            CANVAS_M_RAW(moveTo)
            CANVAS_M_RAW(lineTo)
            CANVAS_M_RAW(arcTo)
            CANVAS_M_RAW(quadTo)
            CANVAS_M_RAW(bezierTo)
            CANVAS_M_RAW(arc)
            CANVAS_M_RAW(closePath)
            CANVAS_M_RAW(rect)
            CANVAS_M_RAW(roundedRect)
            CANVAS_M_RAW(circle)
            CANVAS_M_RAW(ellipse)
            CANVAS_M_RAW(fill)
            CANVAS_M_RAW(stroke)
            CANVAS_M_RAW(fillRect)
            CANVAS_M_RAW(strokeRect)
            CANVAS_M_RAW(clearColor)
            CANVAS_M_RAW(fillText)
            CANVAS_M_RAW(drawImage)
            CANVAS_M_RAW(lineCap)
            CANVAS_M_RAW(lineJoin)
            CANVAS_M_RAW(strokeWidth)
            CANVAS_M_RAW(miterLimit)
            CANVAS_M_RAW(globalAlpha)
            CANVAS_M_RAW(fillColor)
            CANVAS_M_RAW(fillGradient)
            CANVAS_M_RAW(strokeColor)
            CANVAS_M_RAW(strokeGradient)
            CANVAS_M_RAW(font)
            CANVAS_M_RAW(fontSize)
            CANVAS_M_RAW(textAlign)
            CANVAS_M_RAW(textStyle)
            CANVAS_M_RAW(measureText)
            CANVAS_M_RAW(scale)
            CANVAS_M_RAW(rotate)
            CANVAS_M_RAW(translate)
            CANVAS_M_RAW(transform)
            CANVAS_M_RAW(setTransform)
            CANVAS_M_RAW(resetTransform)
            CANVAS_M_RAW(beginFrame)
            CANVAS_M_RAW(cancelFrame)
            CANVAS_M_RAW(endFrame)
            CANVAS_M_RAW(beginPath)
            CANVAS_M_RAW(pathWinding)
            CANVAS_M_RAW(clip)
            CANVAS_M_RAW(clipOut)
            CANVAS_M_RAW(resetClip)
            CANVAS_M_RAW(save)
            CANVAS_M_RAW(restore)
            CANVAS_M_RAW(reset)
            CANVAS_M_RAW(setSize)
            CANVAS_M_RAW(getPosition)
            CANVAS_M_RAW(setPosition)
            CANVAS_M_RAW(offsetPosition)
            CANVAS_M_RAW(setScaleRatio)
            CANVAS_M_RAW(local2Global)
            CANVAS_M_RAW(global2Local)
            CANVAS_M_RAW(getContext)

            {NULL, NULL}
    };
}
DEF__INDEX(Canvas, NanoCanvas::Canvas)
//---------------------- text style ==============
static int TextStyle_font(lua_State* L){
    auto upIndex = lua_upvalueindex(1);
    auto pStyle = LuaUtils::get_ref<NanoCanvas::TextStyle>(L, upIndex);
    auto font = LuaUtils::get_ref<NanoCanvas::Font>(L, 1);
    pStyle->face = font->face;
    lua_pushvalue(L, upIndex);
    return 1;
}
static int TextStyle_color(lua_State* L){
    auto upIndex = lua_upvalueindex(1);
    auto pStyle = LuaUtils::get_ref<NanoCanvas::TextStyle>(L, upIndex);
    auto c = LuaUtils::get_ref<h7::Color>(L, 1);
    pStyle->color = *c;
    lua_pushvalue(L, upIndex);
    return 1;
}
static int TextStyle_hAlign(lua_State* L){
    auto upIndex = lua_upvalueindex(1);
    auto pStyle = LuaUtils::get_ref<NanoCanvas::TextStyle>(L, upIndex);
    pStyle->hAlign = TO_INT(L, 1);
    lua_pushvalue(L, upIndex);
    return 1;
}
static int TextStyle_vAlign(lua_State* L){
    auto upIndex = lua_upvalueindex(1);
    auto pStyle = LuaUtils::get_ref<NanoCanvas::TextStyle>(L, upIndex);
    pStyle->vAlign = TO_INT(L, 1);
    lua_pushvalue(L, upIndex);
    return 1;
}
#define TextStyle_f(name)\
static int TextStyle_##name(lua_State* L){\
    auto upIndex = lua_upvalueindex(1);\
    auto pStyle = LuaUtils::get_ref<NanoCanvas::TextStyle>(L, upIndex);\
    pStyle->name = TO_FLOAT(L, 1);\
    lua_pushvalue(L, upIndex);\
    return 1;\
}
TextStyle_f(size)
TextStyle_f(lineHeight)
TextStyle_f(blur)
TextStyle_f(letterSpace)

namespace sNanoCanvas{
#define TextStyle_M_RAW(name)\
{#name,    TextStyle_##name},
    const static luaL_Reg TextStyle_Methods[] = {
            TextStyle_M_RAW(font)
            TextStyle_M_RAW(size)
            TextStyle_M_RAW(lineHeight)
            TextStyle_M_RAW(blur)
            TextStyle_M_RAW(letterSpace)
            TextStyle_M_RAW(color)
            TextStyle_M_RAW(hAlign)
            TextStyle_M_RAW(vAlign)
            {NULL, NULL}
    };
}
DEF__INDEX(TextStyle, NanoCanvas::TextStyle)
//-------------------------------------------------------------------------
static int Canvas_gc(lua_State *L) {\
    NanoCanvas::Canvas* c = LuaUtils::get_ref<NanoCanvas::Canvas>(L, -1);
    c->unref();
    return 0;
}
#define DEF_GC(type)\
static int type##_gc(lua_State *L) {\
    NanoCanvas::type* c = LuaUtils::get_ref<NanoCanvas::type>(L, -1);\
    delete c;\
    return 0;\
}
//DEF_GC(Canvas)
DEF_GC(Color)
DEF_GC(Gradient)
DEF_GC(Font)
DEF_GC(TextStyle)
DEF_GC(Image)

#define DEF_TOSTRING(type)\
static int type##_tostring(lua_State* L){\
    lua_pushstring(L, #type);\
    return 1;\
}
DEF_TOSTRING(Canvas)
DEF_TOSTRING(Color)
DEF_TOSTRING(Gradient)
DEF_TOSTRING(Font)
DEF_TOSTRING(TextStyle)
DEF_TOSTRING(Image)

namespace gh7{
    const static luaL_Reg Color_Methods[] = {
            {"__gc",              Color_gc},
            {"__tostring",        Color_tostring},
            {NULL, NULL}
    };
}
namespace gNanoCanvas{
    const static luaL_Reg Canvas_Methods[] = {
            {"__index",           Canvas_index},
            {"__tostring",        Canvas_tostring},
            {"__gc",              Canvas_gc},
            {NULL, NULL}
    };
    const static luaL_Reg Gradient_Methods[] = {
            {"__gc",              Gradient_gc},
            {"__tostring",        Gradient_tostring},
            {NULL, NULL}
    };
    const static luaL_Reg Font_Methods[] = {
            {"__gc",              Font_gc},
            {"__tostring",        Font_tostring},
            {NULL, NULL}
    };
    const static luaL_Reg TextStyle_Methods[] = {
            {"__index",           TextStyle_index},
            {"__tostring",        TextStyle_tostring},
            {"__gc",              TextStyle_gc},
            {NULL, NULL}
    };
    const static luaL_Reg Image_Methods[] = {
            {"__gc",              Image_gc},
            {"__tostring",        Image_tostring},
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
            canvas = new NanoCanvas::Canvas(context,lua_tointeger(L,2),
                    lua_tointeger(L, 3), (float)lua_tonumber(L, 4));
        }
            break;
        case 3:{
            NVGcontext* context = nvgCreate(1, viewId);
            canvas = new NanoCanvas::Canvas(context,lua_tointeger(L,2),
                    lua_tointeger(L, 3));
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
    h7::Color * c = nullptr;
    auto t = lua_type(L, -1);
    if(t == LUA_TSTRING){
        c = new h7::Color();
        c->set(lua_tostring(L, -1));
    } else if(t == LUA_TNUMBER){
        c = new h7::Color();
        c->set(TO_UINT(L, -1));
    } else{
        return luaL_error(L, "color only support. string or int");
    }
    LuaUtils::push_ptr(L, c);
    return 1;
}
static int newColorRgba(lua_State *L) {
    h7::Color * c = new h7::Color();
    c->set(TO_INT(L, 1), TO_INT(L, 2),
           TO_INT(L, 3), TO_INT(L, 4)
            );
    LuaUtils::push_ptr(L, c);
    return 1;
}
static int newColorRgbaF(lua_State *L) {
    h7::Color * c = new h7::Color();
    c->set(TO_FLOAT(L, 1), TO_FLOAT(L, 2),
           TO_FLOAT(L, 3), TO_FLOAT(L, 4)
    );
    LuaUtils::push_ptr(L, c);
    return 1;
}
static int newLinearGradient(lua_State *L) {
    //(float x0, float y0, float x1, float y1,  const Color& scolor , const Color& ecolor)
    if(lua_gettop(L) < 6){
        return luaL_error(L, "wrong arguments for newLinearGradient. expect params are (float x0, float y0, float x1, float y1,"
                             " const Color& scolor , const Color& ecolor)");
    }
    auto scolor = LuaUtils::get_ref<h7::Color>(L, 5);
    auto ecolor = LuaUtils::get_ref<h7::Color>(L, 6);
    auto gradient = NanoCanvas::Canvas::createLinearGradient(TO_FLOAT(L, 1), TO_FLOAT(L, 2),
                                                             TO_FLOAT(L, 3), TO_FLOAT(L, 4),
                                                             *scolor, *ecolor);
    LuaUtils::push_ptr(L, gradient);
    return 1;
}
static int newRadialGradient(lua_State *L) {
    //(float cx, float cy, float r1, float r2,  const Color& scolor , const Color& ecolor)
    if(lua_gettop(L) < 6){
        return luaL_error(L, "wrong arguments for newRadialGradient. expect params are (float cx, float cy, float r1, float r2,"
                             " const Color& scolor , const Color& ecolor)");
    }
    auto scolor = LuaUtils::get_ref<h7::Color>(L, 5);
    auto ecolor = LuaUtils::get_ref<h7::Color>(L, 6);
    auto gradient = NanoCanvas::Canvas::createRadialGradient(TO_FLOAT(L, 1), TO_FLOAT(L, 2),
                                                             TO_FLOAT(L, 3), TO_FLOAT(L, 4),
                                                             *scolor, *ecolor);
    LuaUtils::push_ptr(L, gradient);
    return 1;
}
static int newBoxGradient(lua_State *L) {
    //float x, float y, float w, float h,     float r, float f, Color icol, Color ocol
    if(lua_gettop(L) < 8){
        return luaL_error(L, "wrong arguments for newBoxGradient. expect params are (float x0, float y0, float x1, float y1,"
                             " const Color& scolor , const Color& ecolor)");
    }
    auto icol = LuaUtils::get_ref<h7::Color>(L, 7);
    auto ocol = LuaUtils::get_ref<h7::Color>(L, 8);
    auto gradient = NanoCanvas::Canvas::createBoxGradient(TO_FLOAT(L, 1), TO_FLOAT(L, 2),
                                                             TO_FLOAT(L, 3), TO_FLOAT(L, 4),
                                                             TO_FLOAT(L, 5), TO_FLOAT(L, 6),
                                                          *icol, *ocol);
    LuaUtils::push_ptr(L, gradient);
    return 1;
}
static int newImageGradient(lua_State *L) {
    //(const Image &image, float ox, float oy,float w, float h, float angle, float alpha)
    if(lua_gettop(L) < 6){
        return luaL_error(L, "wrong arguments for newImageGradient. expect params are (const Image &image, float ox, float oy,float w, float h, float angle, float alpha)");
    }
    NanoCanvas::Image* img = LuaUtils::get_ref<NanoCanvas::Image>(L, 1);
    auto gradient = NanoCanvas::Canvas::createImageGradient(*img, TO_FLOAT(L, 1), TO_FLOAT(L, 2),
                                                             TO_FLOAT(L, 3), TO_FLOAT(L, 4),
                                                            TO_FLOAT(L, 3), TO_FLOAT(L, 4));
    LuaUtils::push_ptr(L, gradient);
    return 1;
}
static int newTextStyle(lua_State *L){
    auto pStyle = new NanoCanvas::TextStyle();
    LuaUtils::push_ptr(L, pStyle);
    return 1;
}
static int newFont(lua_State *L){
    //(NVGcontext* ctx, const char* fname , const char* ttfPath)
    if(lua_gettop(L) < 3){
        return luaL_error(L, "wrong arguments for newFont. expect params are (NVGcontext* ptr, const char* fname/Memory , const char* ttfPath)");
    }
    NVGcontext* ctx = LuaUtils::get_ref<NVGcontext>(L, 1);
    NanoCanvas::Font* pFont;
    if(lua_type(L, -1) == LUA_TSTRING){
        pFont = new NanoCanvas::Font(ctx, lua_tostring(L, 2), lua_tostring(L, 3));
    } else{
        SkMemory* mem = LuaUtils::get_ref<SkMemory>(L, 3);
        pFont = new NanoCanvas::Font(ctx, lua_tostring(L, 2), *mem);
    }
    LuaUtils::push_ptr(L, pFont);
    return 1;
}
static int newImage(lua_State *L){
    //(NVGcontext* ptr, const char* filePath, int imageFlags = 0)
    int top = lua_gettop(L);
    if(top < 2){
        return luaL_error(L, "wrong arguments for newImage. expect params are (NVGcontext* ptr, const char* fname , const char* ttfPath)");
    }
    NVGcontext* ctx = LuaUtils::get_ref<NVGcontext>(L, 1);
    NanoCanvas::Image* img;
    switch (top){
        case 2:{
            if(lua_type(L, 2) == LUA_TSTRING){
                img = new NanoCanvas::Image(ctx, lua_tostring(L, 2));
            } else{
                SkMemory* mem = LuaUtils::get_ref<SkMemory>(L, 2);
                img = new NanoCanvas::Image(ctx, *mem);
            }
        }
            break;
        case 3:{
            if(lua_type(L, 2) == LUA_TSTRING){
                img = new NanoCanvas::Image(ctx, lua_tostring(L, 2), lua_tointeger(L, 3));
            } else{
                SkMemory* mem = LuaUtils::get_ref<SkMemory>(L, 2);
                img = new NanoCanvas::Image(ctx, *mem, lua_tointeger(L, 3));
            }
        }
            break;

        case 4:{
            SkMemory* mem = LuaUtils::get_ref<SkMemory>(L, 4);
            img = new NanoCanvas::Image(ctx, TO_INT(L, 2), TO_INT(L,3), *mem);
        }
            break;
        case 5:{
            SkMemory* mem = LuaUtils::get_ref<SkMemory>(L, 4);
            img = new NanoCanvas::Image(ctx, TO_INT(L, 2), TO_INT(L,3), *mem, TO_INT(L, 5));
        }
            break;

        default:
            return luaL_error(L, "wrong arguments for newImage.");
    }
    LuaUtils::push_ptr(L, img);
    return 1;
}
#ifdef H_INTERNAL_TEST
#include "nanovg/nanovg.h"
#include "font/utf8.h"
#include "SkUTF.h"
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
static int h_testDraw(lua_State *L){
    NVGcontext* ctx = LuaUtils::get_ref<NVGcontext>(L, 1);
    NVGcontext* vg = ctx;
   //圆角矩形
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, 50, 50,300,30, 10);
    nvgFillColor(ctx, nvgRGBA(255,192,0,255));
    nvgFill(ctx);

    //矩形
    nvgBeginPath(ctx);
    nvgRect(ctx, 100, 100, 120,30);
    nvgFillColor(ctx, nvgRGBA(255,192,0,255));
    nvgFill(ctx);
    //矩形 + 圆
    nvgBeginPath(vg);
    nvgRect(vg, 100,200, 120,30);
    nvgCircle(vg, 200, 200, 20);
    nvgPathWinding(vg, NVG_HOLE);	// Mark circle as a hole. 相交的地方以src为准.
    nvgFillColor(vg, nvgRGBA(255,192,0,255));
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgRect(vg, 100, 300, 120,30);
    nvgCircle(vg, 200,300, 20);
    nvgPathWinding(vg, NVG_SOLID);//相交的地方以dst为准.
    nvgFillColor(vg, nvgRGBA(255,192,0,255));
    nvgFill(vg);

    //TODO how to render chinese?
    const char* _string = "中";
    int32_t codepoint = *(unsigned char *)_string;
    //UTF8_ACCEPT = 0
    uint16_t utf16[2];
    char arr[4];
    if(SkUTF::ToUTF8(codepoint, arr) > 0){
       // e4d, 0d
        LOGD("ToUTF16 ok: %d", codepoint);
    }
    LOGD("长度= %d", strlen(_string));
    return 0;
}
#endif

static const luaL_Reg mem_funcs[] = {
        {"newCanvas", newCanvas},
        {"newColor",  newColor},
        {"newColorRgba",  newColorRgba},
        {"newColorRgbaF",  newColorRgbaF},
        {"newTextStyle",  newTextStyle},
        {"newLinearGradient",  newLinearGradient},
        {"newRadialGradient",  newRadialGradient},
        {"newBoxGradient",     newBoxGradient},
        {"newImageGradient",   newImageGradient},
        //depend on nvgContext
        {"newFont",            newFont},
        {"newImage",           newImage},
#ifdef H_INTERNAL_TEST
        {"h_testDraw",           h_testDraw},
#endif
        {nullptr,     nullptr}
};

extern "C" int luaopen_canvas_lua(lua_State *L) {
    CanvasLua::registers(L);
    luaL_newlib(L, mem_funcs);
    return 1;
}

DEF_MTNAME(NanoCanvas::Canvas)
DEF_MTNAME(h7::Color)
DEF_MTNAME(NanoCanvas::Gradient)
DEF_MTNAME(NanoCanvas::Font)
DEF_MTNAME(NanoCanvas::TextStyle)
DEF_MTNAME(NanoCanvas::Image)
DEF_MTNAME(NVGcontext)
void CanvasLua::registers(lua_State *L) {
    REG_CLASS(L, NanoCanvas::Canvas);
    REG_CLASS(L, h7::Color);
    REG_CLASS(L, NanoCanvas::Gradient);
    REG_CLASS(L, NanoCanvas::Font);
    REG_CLASS(L, NanoCanvas::TextStyle);
    REG_CLASS(L, NanoCanvas::Image);
    REG_EMPTY_CLASS(L, NVGcontext);
}