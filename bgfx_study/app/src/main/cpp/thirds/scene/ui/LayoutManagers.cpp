//
// Created by Administrator on 2020/12/31 0031.
//

#include "LayoutManagers.h"
#include "global.h"

namespace h7{
    void LinearLayoutManager::measureImpl(h7::ListLayout *layout, int offsetCount,float restrictW, float restrictH,
                                          float& w, float& h) {
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
        if(lastIndex < 0){
            lastIndex = offsetCount;
        }

        switch (layout->getScrollMode()){
            case ScrollInfo::SCROLL_MODE_FLING:
                if(vertical){
                    //scroll up means content scroll down
                    if(layout->getScrollDir()== ScrollInfo::SCROLL_DIRECTION_UP){
                        lastIndex += 3 * offsetCount;
                    } else{
                        firstIndex = bx::max(firstIndex - 3 * offsetCount, 0);
                    }
                } else{
                    if(layout->getScrollDir()== ScrollInfo::SCROLL_DIRECTION_LEFT){
                        lastIndex += 3 * offsetCount;
                    } else{
                        firstIndex = bx::max(firstIndex - 3 * offsetCount, 0);
                    }
                }
                break;
            case ScrollInfo::SCROLL_MODE_SCROLLING:
                if(vertical){
                    //scroll up means content scroll down
                    if(layout->getScrollDir()== ScrollInfo::SCROLL_DIRECTION_UP){
                        lastIndex += offsetCount;
                    } else{
                        firstIndex = bx::max(firstIndex - offsetCount, 0);
                    }
                } else{
                    if(layout->getScrollDir()== ScrollInfo::SCROLL_DIRECTION_LEFT){
                        lastIndex += offsetCount;
                    } else{
                        firstIndex = bx::max(firstIndex - offsetCount, 0);
                    }
                }
                break;
        }
        //pre measured
        /*  for (int i = 0; i < firstIndex; ++i) {
              auto wh = pRecycler->getMeasures(i);
              if(vertical){
                  totalH += wh.h;
                  totalW = bx::max(wh.w, totalW);
              } else{
                  totalH = bx::max(wh.h, totalH);
                  totalW += wh.w;
              }
          }*/
        for (int i = firstIndex; i <= lastIndex; ++i) {
            auto item = pAdapter->getItem(i);
            auto pHolder = layout->findViewHolder(pAdapter->getViewType(i, item));
            pHolder->itemView->weak_ref();
            if(pHolder->itemView->try_ref()){
                pHolder->itemView->getMargin(margin);
                if(pHolder->itemView->hasActorType(H7_LAYOUT_TYPE)){
                    auto actor = rCast(Layout*, pHolder->itemView.get());
                    actor->setNeedMeasure(true);
                    actor->doMeasure(restrictW, restrictH, tempW, tempH);
                    pRecycler->setMeasures(i, tempW + margin.horizontal(), tempH + margin.vertical());
                }
                pHolder->itemView->unref();
                if(vertical){
                    totalH += tempH + margin.vertical();
                    totalW = bx::max(tempW + margin.horizontal(), totalW );
                } else{
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
    void LinearLayoutManager::measure(ListLayout *layout, float restrictW, float restrictH,float &w, float &h) {
        auto pAdapter = cCast_ref(ListAdapter*, layout->getAdapter());
        if(pAdapter == nullptr){
            w = 0;
            h = 0;
        } else{
            auto pRecycler = cCast_ref(Recycler*, layout->getRecycler());
            auto offsetCount = pRecycler->getOffsetCount(pAdapter);
            auto margin = SkRect::MakeEmpty();
            //3 * offsetCount
            if(offsetCount > 0){
                measureImpl(layout, offsetCount, restrictW, restrictH, w, h);
            } else{
                //measure/compute the offset
                auto c = pAdapter->getItemCount();
                if(c == 0){
                    w = 0;
                    h = 0;
                    return;
                }
                float totalH = 0;
                float totalW = 0;
                float tempW = 0, tempH = 0;

                for (int i = 0; i < c; ++i) {
                    auto item = pAdapter->getItem(i);
                    auto pHolder = layout->findViewHolder(pAdapter->getViewType(i, item));
                    pHolder->itemView->weak_ref();
                    if(pHolder->itemView->try_ref()){
                        pHolder->itemView->getMargin(margin);
                        if(pHolder->itemView->hasActorType(H7_LAYOUT_TYPE)){
                            auto actor = rCast(Layout*, pHolder->itemView.get());
                            actor->setNeedMeasure(true);
                            actor->doMeasure(restrictW, restrictH, tempW, tempH);
                        }
                        pHolder->itemView->unref();
                        if(vertical){
                            totalH += tempH + margin.vertical();
                            totalW = bx::max(tempW + margin.horizontal(), totalW);
                            if(totalH >= restrictH){
                                offsetCount = i + 3;
                                break;
                            }
                        } else{
                            totalW += tempW + margin.horizontal();
                            totalH = bx::max(tempH + margin.vertical(), totalH);
                            if(totalW >= restrictW){
                                offsetCount = i + 3;
                                break;
                            }
                        }
                    }
                    pHolder->itemView->weak_unref();
                }
                //can't reach the max
                if(offsetCount <= 0 ){
                    w = totalW;
                    h = totalH;
                    return;
                }
                measureImpl(layout, offsetCount, restrictW, restrictH, w, h);
            }
        }
    }

    void LinearLayoutManager::layoutChildren(ListLayout *layout, float targetX, float targetY, float w, float h) {
        
    }
}