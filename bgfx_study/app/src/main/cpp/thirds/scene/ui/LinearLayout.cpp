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

        auto margin = SkRect::MakeEmpty();
        float totalMargin = 0;

        for (int i = 0, n = array.size(); i < n; i++) {
            const sk_sp<Actor> &sp = array.get(i);
            sp->getMargin(margin);
            if(vertical){
                totalMargin += margin.fTop + margin.fBottom;
            } else{
                totalMargin += margin.fLeft + margin.fRight;
            }
            if(sp->hasActorType(H7_LAYOUT_TYPE)){
                auto lp = rCast(Layout*, sp.get())->getLayoutParams();
                if(lp->lpType == LP_TYPE_LINEAR){
                    weights.add(_tmpWeight = rCast(LinearLayoutParams* , lp)->weight);
                    sum += _tmpWeight;
                    continue;
                }
            }
            sum = 0;
            totalMargin = 0;
            break;
        }
        //layout
        if(vertical){
            for (int i = 0, n = array.size(); i < n; i++) {
                const sk_sp<Actor> &sp = array.get(i);
                //set height by weight
                if(sum > 0){
                    sp->setHeight((h - totalMargin) * weights.get(i) / sum);
                }
                sp->getMargin(margin);
                Align::applyAlign(range, sp->getWidth(), sp->getHeight(), align, left, top);
                sp->doLayout(left, top, w, h);
                range.fTop = top + sp->getHeight() + margin.fTop + margin.fBottom;
            }
        } else{
            for (int i = 0, n = array.size(); i < n; i++) {
                const sk_sp<Actor> &sp = array.get(i);
                //set width by weight
                if(sum > 0){
                    sp->setWidth((w - totalMargin) * weights.get(i) / sum);
                }
                sp->getMargin(margin);
                Align::applyAlign(range, sp->getWidth(), sp->getHeight(), align, left, top);
                sp->doLayout(left, top, w, h);
                range.fLeft = left + sp->getWidth() + margin.fLeft + margin.fRight;
            }
        }
    }
    void LinearLayout::measure(float &outW, float &outH) {
        auto array = children.copy();
        float measureW = 0, measureH = 0;
        float w = 0, h = 0;
        auto margin = SkRect::MakeEmpty();

        if(vertical){
            for (int i = 0, n = array.size(); i < n; i++) {
                const sk_sp<Actor> &sp = array.get(i);
                sp->getMargin(margin);
                if(sp->hasActorType(H7_LAYOUT_TYPE)){
                    rCast(Layout*, sp.get())->doMeasure(measureW, measureH);
                    h += measureH + margin.top() + margin.bottom();
                    w = bx::max(w, measureW + margin.left() + margin.right());
                }
            }
        } else{
            for (int i = 0, n = array.size(); i < n; i++) {
                const sk_sp<Actor> &sp = array.get(i);
                sp->getMargin(margin);
                if(sp->hasActorType(H7_LAYOUT_TYPE)){
                    rCast(Layout*, sp.get())->doMeasure(measureW, measureH);
                    w += measureW + margin.left() + margin.right();
                    h = bx::max(h, measureH + margin.top() + margin.bottom());
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
            requestLayoutAndInvalidate();
        }
    }
}