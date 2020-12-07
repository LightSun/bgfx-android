//
// Created by Administrator on 2020/12/6 0006.
//

#ifndef BGFX_STUDY_DRAWABLE_H
#define BGFX_STUDY_DRAWABLE_H

#include "../nancanvas/SkRect.h"
#include "Align.h"
#include "lua/SkRefCnt.h"

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
    public:
        /**
         * called to draw drawable
         * @param canvas the canvas
         * @param x the x of cors
         * @param y the y of cors
         */
        virtual void draw(NanoCanvas::Canvas& canvas, float x, float y, float width, float height){
            _tmp.setXYWH(0, 0, width, height);
            if(!getBounds().isEmpty()){
                Align::applyAlign(_tmp, getBounds(), align, _tmp);
            }
            onDraw(canvas, x, y ,width, height);
        };
        virtual void onDraw(NanoCanvas::Canvas& canvas, float x, float y, float width, float height){

        };
        SkRect& getBounds();
        virtual void setBounds(SkRect& in);

        unsigned char getLevel() const;

        void setLevel(unsigned char level);

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
