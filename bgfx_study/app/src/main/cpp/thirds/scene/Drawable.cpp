//
// Created by Administrator on 2020/12/6 0006.
//
#include "Drawable.h"


namespace h7{

    void Drawable::draw(NanoCanvas::Canvas& canvas, float x, float y, float width, float height){
        _tmp.setXYWH(0, 0, width, height);
        if(!getBounds().isEmpty()){
            Align::applyAlign(_tmp, getBounds(), align, _tmp);
        }
        onDraw(canvas, x, y ,width, height);
    };

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
    void Drawable::setAlign(int align){
        this->align = align;
    }
    int Drawable::getAlign(){
        return align;
    }
    unsigned char Drawable::getLevel() const {
        return _level;
    }
    void Drawable::setLevel(unsigned char level) {
        _level = level;
    }

}