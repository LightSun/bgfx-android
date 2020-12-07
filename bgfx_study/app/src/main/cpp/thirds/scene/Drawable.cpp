//
// Created by Administrator on 2020/12/6 0006.
//
#include "Drawable.h"


namespace h7{

    SkRect & Drawable::getBounds() {
        return _bounds;
    }
    void Drawable::setBounds(SkRect &in) {
        _bounds.set(in);
    }

    float Drawable::getRight() {
        return _bounds.fRight;
    }
    float Drawable::getTop() {
        return _bounds.fTop;
    }
    float Drawable::getLeft() {
        return _bounds.fLeft;
    }
    float Drawable::getBottom() {
        return _bounds.fBottom;
    }

    unsigned char Drawable::getLevel() const {
        return _level;
    }
    void Drawable::setLevel(unsigned char level) {
        _level = level;
    }

}