//
// Created by Administrator on 2020/12/6 0006.
//

#ifndef BGFX_STUDY_DRAWABLE_H
#define BGFX_STUDY_DRAWABLE_H

#include "lua/SkRefCnt.h"
#include "Align.h"
#include "../nancanvas/SkRect.h"

namespace NanoCanvas {
    class Canvas;
}

namespace h7{

    class Drawable : public SkRefCnt{
    private:
        SkRect _bounds;
        int align = Align::center;
        unsigned char _level = 100; //0-100

        SkRect realRect;

    protected:
        SkRect* _pad;//temp padding just used for draw

        /**
        * called to draw drawable
        * @param canvas the canvas
        * @param x the x of screen x
        * @param y the y of screen y
        * @param real the real rect
        */
        virtual void onDraw(NanoCanvas::Canvas& canvas, float x, float y, SkRect& real){

        };
    public:
        /**
         * called to draw drawable
         * @param canvas the canvas
         * @param x the x of screen x
         * @param y the y of screen y
         * @param width limit width
         * @param height limit height
         */
        virtual void draw(NanoCanvas::Canvas& canvas, float x, float y, float width, float height);

        SkRect& getBounds();
        virtual void setBounds(SkRect& in);

        virtual void setPadding(SkRect* pad);
        SkRect& getPadding(SkRect& rect);

        unsigned char getLevel() const;

        virtual void setLevel(unsigned char level);

        virtual void setAlign(int align);
        int getAlign();

        float getLeft();
        float getRight();
        float getTop();
        float getBottom();
    };
}

#endif //BGFX_STUDY_DRAWABLE_H
