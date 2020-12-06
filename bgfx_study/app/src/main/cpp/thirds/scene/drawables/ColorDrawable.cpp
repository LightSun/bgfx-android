//
// Created by Administrator on 2020/12/6 0006.
//

#include "ColorDrawable.h"
#include "../../nancanvas/Canvas.h"

namespace h7{

    void ColorDrawable::setRound(float round) {
        _round = round;
    }
    void ColorDrawable::setCircle(bool circle) {
        _circle = circle;
    }
    void ColorDrawable::setColor(h7::Color &in) {
        _color.set(in);
    }
    void ColorDrawable::draw(NanoCanvas::Canvas &canvas, float x, float y) {
        auto rect = getBounds();
        float r = _circle ? rect.width() / 2 : _round;
        canvas.beginPath()
                .roundedRect(x + rect.fLeft, y + rect.fTop, rect.width(), rect.height(), r)
                .fillColor(_color)
                .fill();
    }
}