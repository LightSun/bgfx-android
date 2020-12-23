//
// Created by Administrator on 2020/12/23 0023.
//

#include "Widget.h"
#include "SceneHelper.h"

namespace h7{

    void Widget::validate() {
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
        if (!_needsLayout) return;
        _needsLayout = false;
        layout();
    }
    void Widget::invalidateHierarchy() {
        if (!layoutEnabled) return;
        invalidate();
        //all parent should impl layout
        auto parent = getParent();
        if(parent->hasActorType(H7_LAYOUT_TYPE)){
            reinterpret_cast<Layout*>(getParent())->invalidateHierarchy();
        }
    }
}