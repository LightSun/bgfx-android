//
// Created by Administrator on 2020/12/31 0031.
//

#include "LayoutManagers.h"
#include "global.h"
#include "input/GestureContext.h"

namespace h7 {
    void LinearLayoutManager::measureImpl(h7::ListLayout *layout, int offsetCount, float restrictW,
                                          float restrictH,
                                          float &w, float &h) {
        auto pAdapter = cCast_ref(ListAdapter*, layout->getAdapter());
        auto pRecycler = cCast_ref(Recycler*, layout->getRecycler());
        auto margin = SkRect::MakeEmpty();

        float totalH = 0;
        float totalW = 0;
        float tempW = 0, tempH = 0;

        /**
         * 1, based on scroll x/y. offsetCount, item heights. compute the first layout position.
         * 2. measure 3* offsetCount item as layout width and height.
         *     or measure height until 3/5*screenHeight
         * 3,
         */
        auto firstIndex = getFirstVisibleItem();
        auto lastIndex = getLastVisibleItem();
        if (lastIndex < 0) {
            lastIndex = offsetCount;
        }

        switch (layout->getScrollMode()) {
            case ScrollInfo::SCROLL_MODE_FLING:
                if (vertical) {
                    //scroll up means content scroll down
                    if (layout->getScrollDir() == ScrollInfo::SCROLL_DIRECTION_UP) {
                        lastIndex += 3 * offsetCount;
                    } else {
                        firstIndex = bx::max(firstIndex - 3 * offsetCount, 0);
                    }
                } else {
                    if (layout->getScrollDir() == ScrollInfo::SCROLL_DIRECTION_LEFT) {
                        lastIndex += 3 * offsetCount;
                    } else {
                        firstIndex = bx::max(firstIndex - 3 * offsetCount, 0);
                    }
                }
                break;
            case ScrollInfo::SCROLL_MODE_SCROLLING:
                if (vertical) {
                    //scroll up means content scroll down
                    if (layout->getScrollDir() == ScrollInfo::SCROLL_DIRECTION_UP) {
                        lastIndex += offsetCount;
                    } else {
                        firstIndex = bx::max(firstIndex - offsetCount, 0);
                    }
                } else {
                    if (layout->getScrollDir() == ScrollInfo::SCROLL_DIRECTION_LEFT) {
                        lastIndex += offsetCount;
                    } else {
                        firstIndex = bx::max(firstIndex - offsetCount, 0);
                    }
                }
                break;
        }
        for (int i = firstIndex; i <= lastIndex; ++i) {
            auto item = pAdapter->getItem(i);
            auto pHolder = layout->findViewHolder(pAdapter->getViewType(i, item), item);
            pHolder->setLayoutPosition(i);
            if(!pHolder->isSamePosition()){
                pAdapter->onBindData(pHolder, i);
            }
            pHolder->itemView->weak_ref();
            if (pHolder->itemView->try_ref()) {
                pHolder->itemView->getMargin(margin);
                if (pHolder->itemView->hasActorType(H7_LAYOUT_TYPE)) {
                    auto actor = rCast(Layout*, pHolder->itemView.get());
                    actor->setNeedMeasure(true);
                    actor->doMeasure(restrictW, restrictH, tempW, tempH);
                    pRecycler->setMeasures(i, tempW + margin.horizontal(),
                                           tempH + margin.vertical());
                }
                pHolder->itemView->unref();
                if (vertical) {
                    totalH += tempH + margin.vertical();
                    totalW = bx::max(tempW + margin.horizontal(), totalW);
                } else {
                    totalW += tempW + margin.horizontal();
                    totalH = bx::max(tempH + margin.vertical(), totalH);
                }
            }
            pHolder->itemView->weak_unref();
        }
        firstLayoutIndex = firstIndex;
        lastLayoutIndex = lastIndex;
        w = totalW;
        h = totalH;
    }

    void
    LinearLayoutManager::measure(ListLayout *layout, float restrictW, float restrictH, float &w,
                                 float &h) {
        auto pAdapter = cCast_ref(ListAdapter*, layout->getAdapter());
        if (pAdapter == nullptr) {
            w = 0;
            h = 0;
        } else {
            auto pRecycler = cCast_ref(Recycler*, layout->getRecycler());
            auto offsetCount = pRecycler->getOffsetCount(pAdapter);
            auto margin = SkRect::MakeEmpty();
            //3 * offsetCount
            if (offsetCount > 0) {
                measureImpl(layout, offsetCount, restrictW, restrictH, w, h);
            } else {
                //measure/compute the offset
                auto c = pAdapter->getItemCount();
                if (c == 0) {
                    w = 0;
                    h = 0;
                    return;
                }
                float totalH = 0;
                float totalW = 0;
                float tempW = 0, tempH = 0;

                for (int i = 0; i < c; ++i) {
                    auto item = pAdapter->getItem(i);
                    auto pHolder = layout->findViewHolder(pAdapter->getViewType(i, item), item);
                    pHolder->setLayoutPosition(i);
                    if(!pHolder->isSamePosition()){
                        pAdapter->onBindData(pHolder, i);
                    }

                    auto sp_itemView = pHolder->itemView;
                    sp_itemView->weak_ref();
                    if (sp_itemView->try_ref()) {
                        sp_itemView->getMargin(margin);
                        if (sp_itemView->hasActorType(H7_LAYOUT_TYPE)) {
                            auto actor = rCast(Layout*, sp_itemView.get());
                            actor->setNeedMeasure(true);
                            actor->doMeasure(restrictW, restrictH, tempW, tempH);
                        }
                        sp_itemView->unref();
                        if (vertical) {
                            totalH += tempH + margin.vertical();
                            totalW = bx::max(tempW + margin.horizontal(), totalW);
                            if (totalH >= restrictH) {
                                offsetCount = i + 3;
                                break;
                            }
                        } else {
                            totalW += tempW + margin.horizontal();
                            totalH = bx::max(tempH + margin.vertical(), totalH);
                            if (totalW >= restrictW) {
                                offsetCount = i + 3;
                                break;
                            }
                        }
                    }
                    sp_itemView->weak_unref();
                }
                //can't reach the max
                if (offsetCount <= 0) {
                    w = totalW;
                    h = totalH;
                    return;
                }
                measureImpl(layout, offsetCount, restrictW, restrictH, w, h);
            }
        }
    }

    void LinearLayoutManager::layoutChildren(ListLayout *layout, float targetX, float targetY, float w,
                                        float h) {
        auto pRecycler = cCast_ref(Recycler*, layout->getRecycler());
        auto pAdapter = cCast_ref(ListAdapter*, layout->getAdapter());
        //pre measured
        float totalH = 0;
        float totalW = 0;
        for (int i = 0; i < firstLayoutIndex; ++i) {
            auto wh = pRecycler->getMeasures(i);
            if (vertical) {
                totalH += wh.h;
                totalW = bx::max(wh.w, totalW);
            } else {
                totalH = bx::max(wh.h, totalH);
                totalW += wh.w;
            }
        }
        float left, top;
        SkRect range = SkRect::MakeXYWH(targetX, targetY, w, h);
        auto margin = SkRect::MakeEmpty();
        //screen info
        int whs[2];
        getDisplayInfo(whs);

        //visible holders
        pRecycler->visibleHolders.clear();
        for (int i = firstLayoutIndex; i <= lastLayoutIndex; ++i) {
            auto item = pAdapter->getItem(i);
            auto pHolder = layout->getUniqueViewHolder(pAdapter->getViewType(i, item), item);
            //set position and bind data if need
            pHolder->setLayoutPosition(i);
            if(!pHolder->isSamePosition()){
                pAdapter->onBindData(pHolder, i);
            }
            //pHolder->
            //pAdapter->onBindData(pHolder, i);
            auto sp_itemView = pHolder->itemView;
            sp_itemView->weak_ref();
            if (sp_itemView->try_ref()) {
                sp_itemView->getMargin(margin);
                auto lp = rCast(Layout*, sp_itemView.get())->getLayoutParams();
                if(lp->lpType == LP_TYPE_LIST_ITEM){
                    auto itemlp = rCast(ListLayout::ItemLayoutParams*, lp);
                    Align::applyAlign(range, sp_itemView->getWidth(), sp_itemView->getHeight(), itemlp->align, left, top);
                } else{
                    left = range.left();
                    top = range.top();
                }
                sp_itemView->doLayout(left, top, w, h);
                if(vertical){
                    range.fLeft = left;
                    range.fTop = top + sp_itemView->getHeight() + margin.vertical();
                    //set using if not out-of screen.
                    pHolder->setUsing(!((top < 0 && range.top() < 0) ||
                                        (top > whs[1] && range.top() > whs[1])));
                } else{
                    range.fTop = top;
                    range.fLeft = left + sp_itemView->getWidth() + margin.horizontal();
                    //set using if not out-of screen.
                    pHolder->setUsing(!((left < 0 && range.left() < 0) ||
                                        (left > whs[0] && range.left() > whs[0])));
                }
                sp_itemView->unref();
            }
            sp_itemView->weak_unref();
            //the view holder should be draw.
            if(pHolder->isUsing()){
                auto sp = sk_ref_sp(pHolder);
                pRecycler->visibleHolders.add(sp);
            }
        }
    }
}