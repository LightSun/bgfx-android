---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by Administrator.
--- DateTime: 2020/11/1 0001 下午 10:32
---
local canvasL = require("canvas_lua")
local mem = require("memory");

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

-- line caps
m.LINE_CAP_BUTT       = 1;
m.LINE_CAP_NVG_SQUARE = 2;
m.LINE_CAP_NVG_ROUND  = 3;
-- line joins
m.LINE_JOIN_ROUND  = 3;
m.LINE_JOIN_BEVEL  = 4;
m.LINE_JOIN_MITER  = 5;
-- image flags
m.IMAGE_FLAG_GenerateMipmaps = 1;
m.IMAGE_FLAG_RepeatX         = 2;
m.IMAGE_FLAG_RepeatY         = 4;
m.IMAGE_FLAG_FlipY           = 8;
m.IMAGE_FLAG_PreMultiplied   = 16;

-- ui matrix ops ---
m.OP_PRE  = 1;
m.OP_POST = 2;
m.OP_SET  = 3;

--(viewId, width, height, [scaleRatio])
function m.newCanvas(viewId, width, height, scaleRatio)
    return canvasL.newCanvas(viewId, width, height, scaleRatio);
end

function m.newColor(strOrInt)
    return canvasL.newColor(strOrInt)
end
function m.newColorRgba(r, g, b, a)
    return canvasL.newColorRgba(r, g, b, a)
end
function m.newColorRgbaF(r, g, b, a)
    return canvasL.newColorRgbaF(r, g, b, a)
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
--- @param x number The x-coordinate of the upper-left corner of the rectangle
--- @param y number The y-coordinate of the upper-left corner of the rectangle
--- @param w number The width of the rectangle, in pixels
--- @param h number The height of the rectangle, in pixels
--- @param r number The radius of the circle formed by 4 corners of the rounded rectangle
--- @param f number How blurry the border of the rectangle is
--- @param icol userdata The inner color of the gradient
--- @param ocol userdata The outer color of the gradient
--- @return userdata The created gradient style object.
---
function m.newBoxGradient(x, y, w, h, r, f, icol, ocol)
    --(float x, float y, float w, float h,  float r, float f, Color icol, Color ocol)
    return canvasL.newBoxGradient(x, y, w, h, r, f, icol, ocol)
end

function m.newImageGradient(image, x, y, w, h, angle, alpha)
    -- (const Image &image, float ox, float oy, float w, float h, float angle, float alpha)
    return canvasL.newImageGradient(image, x, y, w, h, angle, alpha)
end
-- font -------------------
function m.newFont(context, fname, ttfPath)
    --(nvgContext* context, const char* fname , const char* ttfPath)
    return canvasL.newFont(context, fname, ttfPath);
end
function m.newFontFromAssets(context, fname, ttfpath)
    return canvasL.newFont(context, fname, mem.loadFileFromAssets(ttfpath));
end

-- image ----------------- flags default = 0
function m.newImage(context, filePath, flags)
    --(nvgContext* context,const char* filePath, int imageFlags = 0)
    return canvasL.newImage(context, filePath, flags);
end
function m.newImageWH(context, w, h, filePath, flags)
    --(nvgContext* context,const char* filePath, int imageFlags = 0)
    return canvasL.newImage(context, w, h, mem.loadFile(filePath), flags);
end
function m.newImageFromAssets(context, filePath, flags)
    return canvasL.newImage(context, mem.loadFileFromAssets(filePath), flags);
end

function m.newImageFromAssetsWH(context, w, h, filePath, flags)
    return canvasL.newImage(context, w, h, mem.loadFileFromAssets(filePath), flags);
end

function m.newTextStyle()
    -- contains methods: font, size, lineHeight, blur, letterSpace, color, vAlign, hAlign
    return canvasL.newTextStyle();
end

function m.newMat(initValues)
    -- set, get, mat[i] = a, mat[i],
    -- with pre/post/set: translate, rotate, scale, concat
   return canvasL.newMat(initValues);
end

return m;