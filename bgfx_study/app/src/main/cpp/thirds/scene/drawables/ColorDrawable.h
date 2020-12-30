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
        Color _color;
        float _round;
        bool _circle = false;

    protected:
        virtual void onDraw(NanoCanvas::Canvas& canvas, float x, float y, SkRect& real);
    public:
        Color& getColor() const ;
        virtual void setColor(Color& in);
        virtual void setRound(float round);
        virtual void setCircle(bool circle);
    };
}

#endif //BGFX_STUDY_COLORDRAWABLE_H
