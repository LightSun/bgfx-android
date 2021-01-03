//
// Created by Administrator on 2020/12/31 0031.
//

#include "LayoutManagers.h"
#include "global.h"

namespace h7{
    void LinearLayoutManager::measure(h7::ListLayout *layout, float &w, float &h) {
        auto pAdapter = cCast_ref(ListAdapter*, layout->getAdapter());
        if(pAdapter == nullptr){
            w = 0;
            h = 0;
        } else{
            auto pRecycler = cCast_ref(Recycler*, layout->getRecycler());
            auto offsetCount = pRecycler->getOffsetCount(pAdapter);
            /**
             * 1, based on scroll x/y. offsetCount, item heights. compute the first layout position.
             * 2. measure 3* offsetCount item as layout width and height.
             * 3,
             */

            auto c = pAdapter->getItemCount();
            float totalW = 0;
            float totalH = 0;
            float tempW = 0, tempH = 0;
            auto margin = SkRect::MakeEmpty();
            //TODO how to cache measures? 10000+ items?  measure part?
            for (int i = 0; i < c; ++i) {
                auto item = pAdapter->getItem(i);
                auto pHolder = layout->findViewHolder(pAdapter->getViewType(i, item));
                pHolder->itemView->weak_ref();
                if(pHolder->itemView->try_ref()){
                    pHolder->itemView->getMargin(margin);
                    if(pHolder->itemView->hasActorType(H7_LAYOUT_TYPE)){
                        auto actor = rCast(Layout*, pHolder->itemView.get());
                        actor->setNeedMeasure(true);
                        actor->doMeasure(tempW, tempH);
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
            w = totalW;
            h = totalH;
        }
    }

    void LinearLayoutManager::layoutChildren(ListLayout *layout, float targetX, float targetY, float w, float h) {
        
    }
}