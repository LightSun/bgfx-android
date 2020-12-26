//
// Created by Administrator on 2020/12/26 0026.
//

#include "FrameLayout.h"

namespace h7{
    void FrameLayout::onLayoutChildren(float targetX, float targetY, float w, float h) {
        SkRect range = SkRect::MakeXYWH(targetX, targetY, w, h);
        float left, top;

        auto array = children.copy();
        for (int i = 0, n = array.size(); i < n; i++) {
            const sk_sp<Actor> &sp = array.get(i);
            Align::applyAlign(range, sp->getWidth(), sp->getHeight(), align, left, top);
            sp->doLayout(left, top, w, h);
        }
    }

    void FrameLayout::measure(float &outW, float &outH) {
        auto array = children.copy();
        float measureW, measureH;
        float w = 0, h = 0;
        for (int i = 0, n = array.size(); i < n; i++) {
            const sk_sp<Actor> &sp = array.get(i);
            if(sp->hasActorType(H7_LAYOUT_TYPE)){
                rCast(Layout*, sp.get())->measure(measureW, measureH);
                h = bx::max(h, measureH);
                w = bx::max(w, measureW);
            }
        }
        outW = w;
        outH = h;
    }
}