//
// Created by Administrator on 2020/12/23 0023.
//

#include "Widget.h"
#include "SceneHelper.h"

namespace h7{

    int Widget::getActorType(){
        return H7_ACTOR_TYPE | H7_LAYOUT_TYPE;
    }

    void Widget::setLayoutEnabled(bool enabled) {
        Layout::setLayoutEnabled(enabled);
        if (enabled) invalidateHierarchy();
    }

    void Widget::validate() {
        //set size and layout if need
        if (!layoutEnabled) return;

        Group* parent = getParent();
        if (fillParent && parent != NULL) {
            float parentWidth, parentHeight;
            Stage* stage = getStage();
            if (stage != nullptr && parent == stage->getRoot()) {
                parentWidth = stage->getWidth();
                parentHeight = stage->getHeight();
            } else {
                parentWidth = parent->getWidth();
                parentHeight = parent->getHeight();
            }
            setSize(parentWidth, parentHeight);
        }
        if(_needsLayout){
            _needsLayout = false;
            layout();
        }
    }
    void Widget::invalidateHierarchy() {
        //call invalidate and call parent.invalidateHierarchy().
        if (!layoutEnabled) return;
        invalidate();
        //all parent should impl layout
        auto parent = getParent();
        if(parent->hasActorType(H7_LAYOUT_TYPE)){
            reinterpret_cast<Layout*>(getParent())->invalidateHierarchy();
        }
    }
    void Widget::invalidate() {
        _needsLayout = true; //mark need layout
    }

    void Widget::pack() {
        //size size and layout
        setSize(getPrefWidth(), getPrefHeight());
        validate();
    }

    void Widget::draw(NanoCanvas::Canvas& canvas, float parentAlpha) {
        validate();
        Actor::draw(canvas, parentAlpha);
    }
}