#include "NanoCanvas.h"
#include "nanovg/nanovg.h"

namespace NanoCanvas {

    NVGcolor nvgColor(const Color &color) {
        return nvgRGBA(color.r, color.g, color.b, color.a);
    }

    NVGpaint nvgPaint(Canvas &canvas, const Gradient &paint) {
        NVGpaint nvgPaint;
        switch (paint.type) {
            case Gradient::Type::Linear: {
                float x0 = paint.xx;
                float y0 = paint.yy;
                float x1 = paint.aa;
                float y1 = paint.bb;
                canvas.local2Global(x0, y0);
                canvas.local2Global(x1, y1);
                nvgPaint = nvgLinearGradient(canvas.nvgContext(), x0, y0, x1, y1,
                                             nvgColor(paint.sColor),
                                             nvgColor(paint.eColor));
            }
                break;
            case Gradient::Type::Box: {
                float x = paint.xx;
                float y = paint.yy;
                canvas.local2Global(x, y);
                nvgPaint = nvgBoxGradient(canvas.nvgContext(), x, y,
                                          paint.aa, paint.bb, paint.cc,
                                          paint.dd, nvgColor(paint.sColor),
                                          nvgColor(paint.eColor));
            }
                break;
            case Gradient::Type::Radial: {
                float cx = paint.xx;
                float cy = paint.yy;
                canvas.local2Global(cx, cy);
                nvgPaint = nvgRadialGradient(canvas.nvgContext(), cx, cy,
                                             paint.aa, paint.bb,
                                             nvgColor(paint.sColor),
                                             nvgColor(paint.eColor));
            }
                break;
            case Gradient::Type::ImagePattern: {
                float ox = paint.xx;
                float oy = paint.yy;
                canvas.local2Global(ox, oy);
                nvgPaint = nvgImagePattern(canvas.nvgContext(), ox, oy,
                                           paint.aa, paint.bb,
                                           paint.cc,
                                           paint.imageID,
                                           paint.dd);
            }
                break;
            case Gradient::Type::None:
            default:
                break;
        }
        return nvgPaint;
    }

/*----------------- Propoties ---------------------*/
    Canvas::Canvas(NVGcontext *ctx, float width, float height, float scaleRatio) {
        m_nvgCtx = ctx;
        m_width = width;
        m_height = height;
        m_scaleRatio = scaleRatio;
        m_xPos = m_yPos = 0;
    }


/*-------------------- Style Control -------------------*/

    Canvas &Canvas::globalAlpha(float alpha) {
        nvgGlobalAlpha(m_nvgCtx, alpha);
        return *this;
    }

    Canvas &Canvas::lineCap(LineCap cap) {
        int nvgCap = NVG_BUTT;
        if (cap == LineCap::SQUARE)
            nvgCap = NVG_SQUARE;
        else if (cap == LineCap::ROUND)
            nvgCap = NVG_ROUND;
        nvgLineCap(m_nvgCtx, nvgCap);
        return *this;
    }

    Canvas &Canvas::lineJoin(LineJoin join) {
        int nvgJoin = NVG_BEVEL;
        if (join == LineJoin::ROUND)
            nvgJoin = NVG_ROUND;
        else if (join == LineJoin::MITER)
            nvgJoin = NVG_MITER;
        nvgLineJoin(m_nvgCtx, nvgJoin);
        return *this;
    }

    Canvas &Canvas::strokeWidth(float width) {
        nvgStrokeWidth(m_nvgCtx, width);
        return *this;
    }

    Canvas &Canvas::miterLimit(float limit) {
        nvgMiterLimit(m_nvgCtx, limit);
        return *this;
    }


    Canvas &Canvas::fillColor(const Color &color) {
        nvgFillColor(m_nvgCtx, nvgRGBA(color.r, color.g, color.b, color.a));
        return *this;
    }

    Canvas &Canvas::fillGradient(const Gradient &paint) {
        if (paint.type != Gradient::Type::None) {
            NVGpaint npaint = nvgPaint(*this, paint);
            nvgFillPaint(m_nvgCtx, npaint);
        }
        return *this;
    }

    Canvas &Canvas::strokeGradient(const Gradient &paint) {
        if (paint.type != Gradient::Type::None) {
            NVGpaint npaint = nvgPaint(*this, paint);
            nvgStrokePaint(m_nvgCtx, npaint);
        }
        return *this;
    }

    Canvas &Canvas::strokeColor(const Color &color) {
        nvgStrokeColor(m_nvgCtx, nvgRGBA(color.r, color.g, color.b, color.a));
        return *this;
    }

    Gradient* Canvas::createLinearGradient(float x0, float y0, float x1, float y1,
                                          const Color &scolor, const Color &ecolor) {
        Gradient* gdt = new Gradient();
        gdt->type = Gradient::Type::Linear;
        gdt->xx = x0;
        gdt->yy = y0;
        gdt->aa = x1;
        gdt->bb = y1;
        gdt->sColor = scolor;
        gdt->eColor = ecolor;
        return gdt;
    }

    Gradient* Canvas::createRadialGradient(float cx, float cy, float r1, float r2,
                                          const Color &icolor, const Color &ocolor) {
        Gradient* gdt = new Gradient();
        gdt->type = Gradient::Type::Radial;
        gdt->xx = cx;
        gdt->yy = cy;
        gdt->aa = r1;
        gdt->bb = r2;
        gdt->sColor = icolor;
        gdt->eColor = ocolor;
        return gdt;
    }

    Gradient* Canvas::createBoxGradient(float x, float y, float w, float h,
                                       float r, float f, Color icol, Color ocol) {
        Gradient* gdt = new Gradient();
        gdt->type = Gradient::Type::Box;
        gdt->xx = x;
        gdt->yy = y;
        gdt->aa = w;
        gdt->bb = h;
        gdt->cc = r;
        gdt->dd = f;
        gdt->sColor = icol;
        gdt->eColor = ocol;
        return gdt;
    }

    Gradient* Canvas::createImageGradient(const Image &image, float ox, float oy,
                                         float w, float h, float angle, float alpha) {
        Gradient* gdt = new Gradient();
        gdt->type = Gradient::Type::ImagePattern;
        gdt->imageID = image.imageID;
        gdt->xx = ox;
        gdt->yy = oy;
        gdt->aa = w;
        gdt->bb = h;
        gdt->cc = angle;
        gdt->dd = alpha;
        return gdt;
    }

    Canvas &Canvas::font(const Font &font) {
        if (font.isValid())
            nvgFontFaceId(m_nvgCtx, font.face);
        return *this;
    }

    Canvas &Canvas::fontSize(float size) {
        nvgFontSize(m_nvgCtx, size);
        return *this;
    }

    Canvas &Canvas::textAlign(unsigned int hAlign,unsigned int vAlign) {
        nvgTextAlign(m_nvgCtx, hAlign | vAlign);
        return *this;
    }

    void applyTextStyle(Canvas &canvas, const TextStyle &textStyle) {
        if (textStyle.face >= 0)
            nvgFontFaceId(canvas.nvgContext(), textStyle.face);
        if (textStyle.lineHeight > 0)
            nvgTextLineHeight(canvas.nvgContext(), textStyle.lineHeight);
        if (textStyle.blur > 0)
            nvgFontBlur(canvas.nvgContext(), textStyle.blur);
        if (textStyle.letterSpace > 0)
            nvgTextLetterSpacing(canvas.nvgContext(), textStyle.letterSpace);
        nvgTextAlign(canvas.nvgContext(), textStyle.hAlign | textStyle.vAlign);
        nvgFontSize(canvas.nvgContext(), textStyle.size);
    }

    Canvas &Canvas::textStyle(const TextStyle &textStyle) {
        applyTextStyle(*this, textStyle);
        nvgFillColor(m_nvgCtx, nvgColor(textStyle.color));
        return *this;
    }

    float Canvas::measureText(const char* text, float rowWidth) {
        float width = 0;
        if (rowWidth <= 0)
            width = nvgTextBounds(m_nvgCtx, 0, 0, text, nullptr, nullptr);
        else {
            //left. top, right, bottom.
            float bouds[4]{0};
            width = measureText(text, 0, 0, bouds, rowWidth);
        }
        return width;
    }

    float Canvas::measureText(const char* text, float x, float y,
                              float *bounds, float rowWidth) {
        local2Global(x, y);
        if (rowWidth <= 0)
            nvgTextBounds(m_nvgCtx, x, y, text, nullptr, bounds);
        else
            nvgTextBoxBounds(m_nvgCtx, x, y, rowWidth, text, nullptr, bounds);
        float width = 0;
        if (bounds)
            width = bounds[2] - bounds[0];
        return width;
    }
    void Canvas::measureText(const char *text, float x, float y, float *outW, float *outH, float rowWidth) {
        float bounds[4];
        measureText(text, x, y, bounds, rowWidth);
        *outW = bounds[2] - bounds[0];
        *outH = bounds[3] - bounds[1];
    }

/* ------------------- Basic Path ----------------------*/

    Canvas &Canvas::moveTo(float x, float y) {
        local2Global(x, y);
        nvgMoveTo(m_nvgCtx, x, y);
        return *this;
    }

    Canvas &Canvas::lineTo(float x, float y) {
        local2Global(x, y);
        nvgLineTo(m_nvgCtx, x, y);
        return *this;
    }

    Canvas &Canvas::arcTo(float x1, float y1, float x2, float y2, float r) {
        local2Global(x1, y1);
        local2Global(x2, y2);
        nvgArcTo(m_nvgCtx, x1, y1, x2, y2, r);
        return *this;
    }

    Canvas &Canvas::quadTo(float cpx, float cpy, float x, float y) {
        local2Global(cpx, cpy);
        local2Global(x, y);
        nvgQuadTo(m_nvgCtx, cpx, cpy, x, y);
        return *this;
    }

    Canvas &Canvas::bezierTo(float cp1x, float cp1y,
                                  float cp2x, float cp2y,
                                  float x, float y) {
        local2Global(cp1x, cp1y);
        local2Global(cp2x, cp2y);
        local2Global(x, y);
        nvgBezierTo(m_nvgCtx, cp1x, cp1y, cp2x, cp2y, x, y);
        return *this;
    }

    Canvas &Canvas::arc(float x, float y, float r,
                        float sAngle, float eAngle, bool counterclockwise) {
        local2Global(x, y);
        int dir = counterclockwise ? NVG_CCW : NVG_CW;
        nvgArc(m_nvgCtx, x, y, r, sAngle, eAngle, dir);
        return *this;
    }


    Canvas &Canvas::closePath() {
        nvgClosePath(m_nvgCtx);
        return *this;
    }


/* ------------------- Advance Path --------------------*/


    Canvas &Canvas::rect(float x, float y, float w, float h) {
        local2Global(x, y);
        nvgRect(m_nvgCtx, x, y, w, h);
        return *this;
    }

    Canvas &Canvas::roundedRect(float x, float y, float w, float h, float r) {
        local2Global(x, y);
        nvgRoundedRect(m_nvgCtx, x, y, w, h, r);
        return *this;
    }

    Canvas &Canvas::circle(float cx, float cy, float r) {
        local2Global(cx, cy);
        nvgCircle(m_nvgCtx, cx, cy, r);
        return *this;
    }

    Canvas &Canvas::ellipse(float cx, float cy, float rx, float ry) {
        local2Global(cx, cy);
        nvgEllipse(m_nvgCtx, cx, cy, rx, ry);
        return *this;
    }

/* ------------------- Draw Action ---------------------*/

    Canvas &Canvas::fill() {
        nvgFill(m_nvgCtx);
        return *this;
    }

    Canvas &Canvas::stroke() {
        nvgStroke(m_nvgCtx);
        return *this;
    }

    Canvas &Canvas::fillRect(float x, float y, float w, float h) {
        local2Global(x, y);
        nvgBeginPath(m_nvgCtx);
        nvgRect(m_nvgCtx, x, y, w, h);
        nvgFill(m_nvgCtx);
        return *this;
    }

    Canvas &Canvas::strokeRect(float x, float y, float w, float h) {
        local2Global(x, y);
        nvgBeginPath(m_nvgCtx);
        nvgRect(m_nvgCtx, x, y, w, h);
        nvgStroke(m_nvgCtx);
        return *this;
    }

    Canvas &Canvas::clearColor(const Color &color) {
        nvgCancelFrame(m_nvgCtx);
        nvgFillColor(m_nvgCtx, nvgRGBA(color.r, color.g, color.b, color.a));
        nvgBeginPath(m_nvgCtx);
        nvgRect(m_nvgCtx, m_xPos, m_yPos, m_width, m_height);
        nvgFill(m_nvgCtx);

        return *this;
    }

    Canvas &Canvas::fillText(const char *text, float x, float y, float rowWidth) {
        if (strlen(text)) {
            local2Global(x, y);
            if (rowWidth <= 0)
                nvgText(m_nvgCtx, x, y, text, nullptr);
            else
                nvgTextBox(m_nvgCtx, x, y, rowWidth, text, nullptr);
        }
        return *this;
    }

    Canvas &Canvas::drawImage(Image &image, float x, float y,
                              float width, float height,
                              float sx, float sy, float swidth, float sheight) {
        if (image.isValid()) {
            save();

            local2Global(x, y);
            int w, h;
            image.size(w, h);
            //std::isNan has bugs on android . we removed it and use -1
            if (swidth <= 0)
                swidth = w - sx;
            if (sheight <= 0)
                sheight = h - sy;
            if (width <= 0)
                width = swidth;
            if (height <= 0)
                height = sheight;

            resetClip();
            clip(x, y, width, height);

            float sw = width / swidth;
            float sh = height / sheight;
            float rx, ry, rw, rh;
            rw = w * sw;
            rh = h * sh;
            rx = x - sx * sw;
            ry = y - sy * sh;

            Gradient* pattern = createImageGradient(image, rx, ry, rw, rh, 0, 1.0f);
            fillGradient(*pattern);
            rect(rx, ry, rw, rh).fill();
            restore();
            delete pattern;
        }
        return *this;
    }

/*------------------- State Handling -----------------*/

    Canvas &Canvas::save() {
        nvgSave(m_nvgCtx);
        return *this;
    }

    Canvas &Canvas::restore() {
        nvgRestore(m_nvgCtx);
        return *this;
    }

    Canvas &Canvas::reset() {
        nvgReset(m_nvgCtx);
        return *this;
    }

/*--------------------- Transformations ----------------*/

    Canvas &Canvas::scale(float scalewidth, float scaleheight) {
        nvgScale(m_nvgCtx, scalewidth, scaleheight);
        return *this;
    }

    Canvas &Canvas::rotate(float angle) {
        nvgRotate(m_nvgCtx, angle);
        return *this;
    }

    Canvas &Canvas::translate(float x, float y) {
        nvgTranslate(m_nvgCtx, x, y);
        return *this;
    }

    Canvas &Canvas::transform(float a, float b, float c,
                              float d, float e, float f) {
        nvgTransform(m_nvgCtx, a, b, c, d, e, f);
        return *this;
    }

    Canvas &Canvas::setTransform(float a, float b, float c,
                                 float d, float e, float f) {
        nvgResetTransform(m_nvgCtx);
        nvgTransform(m_nvgCtx, a, b, c, d, e, f);
        return *this;
    }

    Canvas &Canvas::resetTransform() {
        nvgResetTransform(m_nvgCtx);
        return *this;
    }

/*---------------- Canvas Control -----------------*/
    Canvas &Canvas::beginFrame(int windowWidth, int windowHeight) {
        nvgBeginFrame(m_nvgCtx, windowWidth, windowHeight, m_scaleRatio);
        // Clip out side area
        //nvgScissor(m_nvgCtx, m_xPos, m_yPos, m_width, m_height);

        return *this;
    }
    Canvas & Canvas::clipOut(float x, float y, float w, float h) {
        local2Global(x, y);
        nvgScissor(m_nvgCtx, x, y, w, h);
        return *this;
    }

    Canvas &Canvas::cancelFrame() {
        nvgCancelFrame(m_nvgCtx);
        return *this;
    }

    void Canvas::endFrame() {
        nvgEndFrame(m_nvgCtx);
    }

    Canvas &Canvas::beginPath() {
        nvgBeginPath(m_nvgCtx);
        return *this;
    }

    Canvas &Canvas::pathWinding(Winding dir) {
        int windingDir = NVG_CW;
        if (dir == Winding::CCW)
            windingDir = NVG_CCW;
        nvgPathWinding(m_nvgCtx, windingDir);
        return *this;
    }

    Canvas &Canvas::clip(float x, float y, float w, float h) {
        local2Global(x, y);
        nvgIntersectScissor(m_nvgCtx, x, y, w, h);
        return *this;
    }
    Canvas &Canvas::resetClip() {
        nvgResetScissor(m_nvgCtx);
        return *this;
    }
}
