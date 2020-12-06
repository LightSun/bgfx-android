//
// Created by Administrator on 2020/12/6 0006.
//

#ifndef BGFX_STUDY_COLORDRAWABLE_H
#define BGFX_STUDY_COLORDRAWABLE_H

#include "../Drawable.h"
#include "../../nancanvas/Color.h"

namespace h7{
    class ColorDrawable: public Drawable{

    private:
        Color _color = {0};
        float _round;
        bool _circle = false;

    public:
        virtual void draw(NanoCanvas::Canvas& canvas, float x, float y);
        Color& getColor(){
            return _color;
        }
        virtual void setColor(Color& in);
        virtual void setRound(float round);
        virtual void setCircle(bool circle);
    };
}

#endif //BGFX_STUDY_COLORDRAWABLE_H
