//
// Created by Administrator on 2020/12/6 0006.
//

#ifndef BGFX_STUDY_DRAWABLE_H
#define BGFX_STUDY_DRAWABLE_H

#include "../nancanvas/SkRect.h"
#include "Align.h"

namespace NanoCanvas {
    class Canvas;
}

namespace h7{

    class Drawable{

    protected:
        SkRect _bounds;
        SkRect _tmp;
        int align = Align::center;
    public:
        /**
         * called to draw drawable
         * @param canvas the canvas
         * @param x the x of cors
         * @param y the y of cors
         */
        virtual void draw(NanoCanvas::Canvas& canvas, float x, float y, int width, int height){

        };

        SkRect& getBounds();
        virtual void setBounds(SkRect& in);

        virtual void setAlign(int align){
            this->align = align;
        }
        int getAlign(){
            return align;
        }
        float getLeft();
        float getRight();
        float getTop();
        float getBottom();
    };
}

#endif //BGFX_STUDY_DRAWABLE_H
