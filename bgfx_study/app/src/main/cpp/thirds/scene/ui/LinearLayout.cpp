//
// Created by Administrator on 2020/12/25 0025.
//

#include "LinearLayout.h"
#include "../SceneHelper.h"
#include "utils/Array.h"

namespace h7{
    LinearLayoutParams::LinearLayoutParams() {
        lpType = LP_TYPE_LINEAR;
    }

    void LinearLayout::onLayoutChildren(float targetX, float targetY, float w, float h){
        float left, top;
        auto array = children.copy();
        SkRect range = SkRect::MakeXYWH(targetX, targetY, w, h);
        //compute weight
        IntArray weights;
        int sum = 0;
        int _tmpWeight;
        for (int i = 0, n = array.size(); i < n; i++) {
            const sk_sp<Actor> &sp = array.get(i);
            if(sp->hasActorType(H7_LAYOUT_TYPE)){
                auto lp = rCast(Layout*, sp.get())->getLayoutParams();
                if(lp->lpType == LP_TYPE_LINEAR){
                    weights.add(_tmpWeight = rCast(LinearLayoutParams* , lp)->weight);
                    sum += _tmpWeight;
                    continue;
                }
            }
            sum = 0;
            break;
        }
        //layout

        if(vertical){
            for (int i = 0, n = array.size(); i < n; i++) {
                const sk_sp<Actor> &sp = array.get(i);
                //set height by weight
                if(sum > 0){
                    sp->setHeight(h * weights.get(i) / sum);
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
                    sp->setWidth(w * weights.get(i) / sum);
                }
                Align::applyAlign(range, sp->getWidth(), sp->getHeight(), align, left, top);
                sp->doLayout(left, top, w, h);
                range.fLeft = left + sp->getWidth();
            }
        }
    }
    void LinearLayout::measure(float &outW, float &outH) {
        auto array = children.copy();
        float measureW, measureH;
        float w = 0, h = 0;
        if(vertical){
            for (int i = 0, n = array.size(); i < n; i++) {
                const sk_sp<Actor> &sp = array.get(i);
                if(sp->hasActorType(H7_LAYOUT_TYPE)){
                    rCast(Layout*, sp.get())->doMeasure(measureW, measureH);
                    h += measureH;
                    w = bx::max(w, measureW);
                }
            }
        } else{
            for (int i = 0, n = array.size(); i < n; i++) {
                const sk_sp<Actor> &sp = array.get(i);
                if(sp->hasActorType(H7_LAYOUT_TYPE)){
                    rCast(Layout*, sp.get())->doMeasure(measureW, measureH);
                    w += measureW;
                    h = bx::max(h, measureH);
                }
            }
        }
        outW = w;
        outH = h;
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