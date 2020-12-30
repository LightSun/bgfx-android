//
// Created by Administrator on 2020/12/28 0028.
//

#include "ListLayout.h"
#include "../SceneHelper.h"

namespace h7{

    void ListLayout::setUpByAdapter() {
        holderMap.clear();
        if(adapter != NULL){
            auto c = adapter->getItemCount();
            for (int i = 0; i < c; ++i) {
                auto item = adapter->getItem(i);
                auto viewType = adapter->getViewType(i, item);
                sk_sp<ItemViewHolder>& holder = holderMap[viewType];
                if(holder == nullptr){
                    holder.reset(adapter->createViewHolder(this, viewType, item));
                }
            }
        }
    }

    void ListLayout::setAdapter(h7::ListAdapter *_adapter) {
        stopScroll();
        adapter.reset(_adapter);
        requestLayoutAndInvalidate();
    }
    void ListLayout::setLayoutManager(h7::LayoutManager *_m) {
        stopScroll();
        layoutManager.reset(_m);
        requestLayoutAndInvalidate();
    }

    void ListLayout::measure(float &outW, float &outH) {
        //TODO
    }

    void ListLayout::onLayoutChildren(float targetX, float targetY, float w, float h) {
        /*
         * 1, setup
         * 2, layout self
         * 3, layout children.
         */
        setUpByAdapter();
        layoutSize(getWidth(), getHeight(), w, h);
        layoutManager->layoutChildren(this, targetX, targetY, w, h);
    }
}