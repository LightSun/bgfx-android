//
// Created by Administrator on 2020/12/6 0006.
//

#include "ColorDrawable.h"
#include "../../nancanvas/Canvas.h"
#include "../Align.h"

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
    void ColorDrawable::draw(NanoCanvas::Canvas &canvas, float x, float y, int width, int height) {
        _tmp.setXYWH(0, 0, width, height);
        Align::applyAlign(_tmp, getBounds(), align, _tmp);

        float r = _circle ? _tmp.width() / 2 : _round;
        canvas.beginPath()
                .roundedRect(x + _tmp.fLeft, y + _tmp.fTop, _tmp.width(), _tmp.height(), r)
                .fillColor(_color)
                .fill();
    }
}