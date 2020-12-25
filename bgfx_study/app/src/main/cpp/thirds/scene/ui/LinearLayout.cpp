//
// Created by Administrator on 2020/12/25 0025.
//

#include "LinearLayout.h"

namespace h7{

    void LinearLayout::setWeightProvider(h7::LinearLayout::WeightProvider *provider) {
        if(weightProvider.get() != provider){
            this->weightProvider.reset(provider);
            layoutAndInvalidate();
        }
    }
    LinearLayout::WeightProvider* LinearLayout::getWeightProvider() {
        return this->weightProvider.get();
    }

    void LinearLayout::onLayoutChildren(float targetX, float targetY, float w, float h){
        float left, top;
        auto array = children.copy();
        SkRect range = SkRect::MakeXYWH(targetX, targetY, w, h);
        int sum = 0;
        if(weightProvider != nullptr){
            for (int i = 0, n = array.size(); i < n; i++) {
                sum += weightProvider->getWeight(*this, i);
            }
        }
        if(vertical){
            for (int i = 0, n = array.size(); i < n; i++) {
                const sk_sp<Actor> &sp = array.get(i);
                //set height by weight
                if(sum > 0){
                    sp->setHeight(h * weightProvider->getWeight(*this, i) / sum);
                }
                Align::applyAlign(range, sp->getWidth(), sp->getHeight(), align, left, top);
                sp->doLayout(left, top, w, h);
                range.fTop = top + sp->getHeight();
            }
        } else{
            for (int i = 0, n = array.size(); i < n; i++) {
                const sk_sp<Actor> &sp = array.get(i);
                //set width by weight
                if(sum > 0){
                    sp->setWidth(w * weightProvider->getWeight(*this, i) / sum);
                }
                Align::applyAlign(range, sp->getWidth(), sp->getHeight(), align, left, top);
                sp->doLayout(left, top, w, h);
                range.fLeft = left + sp->getWidth();
            }
        }
    }
    bool LinearLayout::isVertical() const {
        return vertical;
    }
    void LinearLayout::setVertical(bool vertical) {
        if(this->vertical != vertical){
            LinearLayout::vertical = vertical;
            layoutAndInvalidate();
        }
    }
}