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

    protected:
        SkRect _tmp;
        virtual void onDraw(NanoCanvas::Canvas& canvas, float x, float y, float width, float height){

        };
    public:
        /**
         * called to draw drawable
         * @param canvas the canvas
         * @param x the x of cors
         * @param y the y of cors
         */
        virtual void draw(NanoCanvas::Canvas& canvas, float x, float y, float width, float height);

        SkRect& getBounds();
        virtual void setBounds(SkRect& in);

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
