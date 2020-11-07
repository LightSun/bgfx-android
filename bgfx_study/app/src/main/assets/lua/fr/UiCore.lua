---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by Administrator.
--- DateTime: 2020/11/1 0001 下午 10:32
---
local canvasL = require("canvas_lua")

local m = {};

-- HorizontalAlign
m.TEXT_ALIGN_Left   = 1;
m.TEXT_ALIGN_Center = 2;
m.TEXT_ALIGN_Right  = 4;
-- VerticalAlign
m.TEXT_ALIGN_Top        = 8;
m.TEXT_ALIGN_Middle     = 16;
m.TEXT_ALIGN_Bottom     = 32;
m.TEXT_ALIGN_Baseline   = 64;

--Counter clock wise
m.WINDING_CCW = 1;
-- clock wise
m.WINDING_CW = 2;

--(viewId, width, height, [scaleRatio])
function m.newCanvas(viewId, width, height, scaleRatio)
    --[[ (moveTo)
    (lineTo)
    (arcTo)
    (quadCurveTo)
    (bezierCurveTo)
    (arc)
    (closePath)
    (rect)
    (roundedRect)
    (circle)
    (ellipse)
    (fill)
    (stroke)
    (fillRect)
    (strokeRect)
    (clearColor)
    (fillText)
    (drawImage)
    (lineCap)
    (lineJoin)
    (lineWidth)
    (miterLimit)
    (globalAlpha)
    (fillColor)
    (fillGradient)
    (strokeColor)
    (strokeGradient)
    (font)
    (fontSize)
    (textAlign)
    (textStyle)
    (measureText)
    (scale)
    (rotate)
    (translate)
    (transform)
    (setTransform)
    (resetTransform)
    (beginFrame)
    (cancelFrame)
    (endFrame)
    (beginPath)
    (pathWinding)
    (clip)
    (resetClip)
    (save)
    (restore)
    (reset)
    (setSize)
    (setPosition)
    (setScaleRatio)
    (local2Global)
    (global2Local)]]
    return canvasL.newCanvas(viewId, width, height, scaleRatio);
end

function m.newColor(strOrInt)
    return canvasL.newColor(strOrInt)
end

function m.newLinearGradient(x0, y0, x1, y1, startColor, endColor)
    --(float x0, float y0, float x1, float y1,  const Color& scolor , const Color& ecolor)
    return canvasL.newLinearGradient(x0, y0, x1, y1, startColor, endColor)
end
function m.newRadialGradient(cx, cy, r1, r2, startColor, endColor)
    -- (float cx, float cy, float r1, float r2,  const Color& scolor , const Color& ecolor)
    return canvasL.newRadialGradient(cx, cy, r1, r2, startColor, endColor)
end
---
--- @brief Creates and returns a box gradient.
--- @par Box gradient is a feathered rounded rectangle, it is useful for rendering drop shadows or highlights for boxes.
--- @param x number: The x-coordinate of the upper-left corner of the rectangle
--- @param y number: The y-coordinate of the upper-left corner of the rectangle
--- @param w number: The width of the rectangle, in pixels
--- @param h number: The height of the rectangle, in pixels
--- @param r number: The radius of the circle formed by 4 corners of the rounded rectangle
--- @param f number: How blurry the border of the rectangle is
--- @param icol userdata: The inner color of the gradient
--- @param ocol userdata: The outer color of the gradient
--- @return userdata:The created gradient style object.
---
function m.newBoxGradient(x, y, w, h, r, f, icol, ocol)
    --(float x, float y, float w, float h,  float r, float f, Color icol, Color ocol)
    return canvasL.newBoxGradient(x, y, w, h, r, f, icol, ocol)
end

function m.newImageGradient(image, x, y, w, h, angle, alpha)
    -- (const Image &image, float ox, float oy, float w, float h, float angle, float alpha)
    return canvasL.newImageGradient(image, x, y, w, h, angle, alpha)
end

function m.newFont(canvas, fname, ttfPath)
    --(Canvas& canvas, const char* fname , const char* ttfPath)
    return canvasL.newFont(canvas, fname, ttfPath);
end

function m.newImage(canvas, filePath, flags)
    --(Canvas& canvas,const char* filePath, int imageFlags = 0)
    return canvasL.newImage(canvas, filePath, flags);
end

function m.newTextStyle()
    -- contains methods: font, size, lineHeight, blur, letterSpace, color, vAlign, hAlign
    return canvasL.newTextStyle();
end

return m;