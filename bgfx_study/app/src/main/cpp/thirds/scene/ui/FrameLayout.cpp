//
// Created by Administrator on 2020/12/26 0026.
//

#include "FrameLayout.h"
#include "../SceneHelper.h"

namespace h7{
    void FrameLayout::onLayoutChildren(float targetX, float targetY, float w, float h) {
        SkRect range = SkRect::MakeXYWH(targetX, targetY, w, h);
        float left, top;
        auto margin = SkRect::MakeEmpty();

        auto array = children.copy();
        for (int i = 0, n = array.size(); i < n; i++) {
            const sk_sp<Actor> &sp = array.get(i);
            sp->getMargin(margin);
            Align::applyAlign(range, sp->getWidth(), sp->getHeight(), align, left, top);
            sp->doLayout(left, top, w, h);
        }
    }

    void FrameLayout::measure(float &outW, float &outH) {
        auto array = children.copy();
        float measureW, measureH;
        float w = 0, h = 0;
        auto margin = SkRect::MakeEmpty();

        for (int i = 0, n = array.size(); i < n; i++) {
            const sk_sp<Actor> &sp = array.get(i);
            sp->getMargin(margin);
            if(sp->hasActorType(H7_LAYOUT_TYPE)){
                rCast(Layout*, sp.get())->doMeasure(measureW, measureH);
                h = bx::max(h, measureH + margin.top() + margin.bottom());
                w = bx::max(w, measureW + margin.left() + margin.right());
            }
        }
        outW = w;
        outH = h;
    }
}