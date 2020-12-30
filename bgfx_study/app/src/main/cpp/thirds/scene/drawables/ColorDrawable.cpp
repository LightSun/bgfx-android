//
// Created by Administrator on 2020/12/6 0006.
//

#include "ColorDrawable.h"
#include "../../nancanvas/Canvas.h"
#include "../Align.h"

namespace h7{

    Color& ColorDrawable::getColor(){
        return _color;
    }
    bool ColorDrawable::isCircle() {
        return _circle;
    }
    float ColorDrawable::getRound() {
        return _round;
    }

    void ColorDrawable::setRound(float round) {
        _round = round;
    }
    void ColorDrawable::setCircle(bool circle) {
        _circle = circle;
    }
    void ColorDrawable::setColor(h7::Color &in) {
        _color.set(in);
    }
    void ColorDrawable::onDraw(NanoCanvas::Canvas &canvas, float x, float y, SkRect& real) {
        float r = _circle ? real.width() / 2 : _round;
        canvas.beginPath()
                .roundedRect(x + real.fLeft, y + real.fTop, real.width(), real.height(), r)
                .fillColor(_color)
                .fill();
    }
}