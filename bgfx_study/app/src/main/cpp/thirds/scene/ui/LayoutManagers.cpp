//
// Created by Administrator on 2020/12/31 0031.
//

#include "LayoutManagers.h"
#include "global.h"
#include "input/GestureContext.h"

namespace h7{
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

            } else{
                int screenXY[2];
                getDisplayInfo(screenXY);
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
                            if(totalH >= screenXY[1]){
                                offsetCount = i + 3;
                                break;
                            }
                        } else{
                            totalW += tempW + margin.horizontal();
                            totalH = bx::max(tempH + margin.vertical(), totalH);
                            if(totalW >= screenXY[0]){
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
                /**
                 * 1, based on scroll x/y. offsetCount, item heights. compute the first layout position.
                 * 2. measure 3* offsetCount item as layout width and height.
                 *     or measure height until 3/5*screenHeight
                 * 3,
                 */
                auto firstIndex = pRecycler->getFirstVisibleItem();
                auto lastIndex = pRecycler->getLastVisibleItem();
                if(layout->isScrolling()){
                    if(vertical){
                        //scroll up means content scroll down
                        /*if(layout->isScrollUp()){

                        }*/
                    }
                }

                totalW = 0;
                totalH = 0;
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
                            actor->doMeasure(restrictW, restrictH,tempW, tempH);
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
    }

    void LinearLayoutManager::layoutChildren(ListLayout *layout, float targetX, float targetY, float w, float h) {
        
    }
}