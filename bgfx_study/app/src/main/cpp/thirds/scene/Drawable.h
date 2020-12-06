//
// Created by Administrator on 2020/12/6 0006.
//

#ifndef BGFX_STUDY_DRAWABLE_H
#define BGFX_STUDY_DRAWABLE_H

#include "../nancanvas/SkRect.h"

namespace NanoCanvas {
    class Canvas;
}

namespace h7{

    class Drawable{

    protected:
        SkRect _bounds;

    public:
        /**
         * called to draw drawable
         * @param canvas the canvas
         * @param x the x of cors
         * @param y the y of cors
         */
        virtual void draw(NanoCanvas::Canvas& canvas, float x, float y){

        };

        SkRect& getBounds();
        void setBounds(SkRect& in);

        float getLeft();
        float getRight();
        float getTop();
        float getBottom();
    };
}

#endif //BGFX_STUDY_DRAWABLE_H
