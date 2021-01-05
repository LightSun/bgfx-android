//
// Created by Administrator on 2020/12/28 0028.
//

#include "ListLayout.h"
#include "../SceneHelper.h"

namespace h7{

    void ListLayout::setUpByAdapter() {
        cCast_ref(Recycler*, getRecycler())->reset();
        holderMap.clear();
        if(adapter != NULL){
            auto c = adapter->getItemCount();
            for (int i = 0; i < c; ++i) {
                auto item = adapter->getItem(i);
                auto viewType = adapter->getViewType(i, item);
                sk_sp<ItemViewHolder>* holder = &holderMap[viewType];
                if(holder == nullptr){
                    holderMap[viewType] = sk_ref_sp(adapter->createViewHolder(this, viewType, item));
                }
            }
        }
    }

    void ListLayout::setAdapter(h7::ListAdapter *_adapter) {
        stopScroll();
        adapter.reset(_adapter);
        setUpByAdapter();
        requestLayoutAndInvalidate();
    }
    void ListLayout::setLayoutManager(h7::LayoutManager *_m) {
        stopScroll();
        layoutManager.reset(_m);
        requestLayoutAndInvalidate();
    }

    void ListLayout::measure(float restrictW, float restrictH,float &outW, float &outH) {
        layoutManager->measure(this, restrictW, restrictH, outW, outH);
    }

    void ListLayout::onLayoutChildren(float targetX, float targetY, float w, float h) {
        layoutManager->layoutChildren(this, targetX, targetY, w, h);
    }
    void ListLayout::onScrollChanged(float dx, float dy) {

    }

    ItemViewHolder* ListLayout::findViewHolder(int viewType) {
        return holderMap[viewType].get();
    }
    const ListAdapter* ListLayout::getAdapter(){
        return adapter.get();
    }
    const Recycler* ListLayout::getRecycler(){
        return recycler.get();
    }
    ListLayout::ListLayout() {
        //TODO
    }
}