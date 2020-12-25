//
// Created by Administrator on 2020/12/6 0006.
//
#include "Drawable.h"


namespace h7{

    void Drawable::draw(NanoCanvas::Canvas& canvas, float x, float y, float width, float height){
        realRect.setXYWH(0, 0, width, height);
        float dw, dh;
        if(_pad != nullptr){
            dw = _pad->left() + _pad->right();
            dh = _pad->top() + _pad->bottom();
        } else{
            dw = 0;
            dh = 0;
        }
        if(!getBounds().isEmpty()){
            Align::applyAlign(realRect, getBounds().width()- dw,
                        getBounds().height() - dh,
                        align, realRect);
        } else{
            Align::applyAlign(realRect, width - dw,
                              height - dh, align, realRect);
        }
        onDraw(canvas, x, y, realRect);
    };

    SkRect & Drawable::getBounds() {
        return _bounds;
    }
    void Drawable::setBounds(SkRect &in) {
        _bounds.set(in);
    }
    void Drawable::setPadding(SkRect* pad){
        this->_pad = pad;
    }
    SkRect & Drawable::getPadding(SkRect &rect) {
        if(_pad == nullptr){
            rect.setLTRB(0, 0 , 0 ,0);
        } else{
            rect.set(*_pad);
        }
        return rect;
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