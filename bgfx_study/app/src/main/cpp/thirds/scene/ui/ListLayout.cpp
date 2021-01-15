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

    ItemViewHolder * ListLayout::findViewHolder(int viewType, ListItemDelegate *item) {
        sk_sp<ItemViewHolder>* holder = &holderMap[viewType];
        if(holder == nullptr){
            auto sp_holder = sk_ref_sp(adapter->createViewHolder(this, viewType, item));
            holderMap[viewType] = sp_holder;
            holder = &sp_holder;
        }
        return holder->get();
    }
    ItemViewHolder* ListLayout::getUniqueViewHolder(int viewType, h7::ListItemDelegate *item) {
        Array<sk_sp<ItemViewHolder>>* holders = layoutHolderMap[viewType];
        if(holders == nullptr){
            holders = layoutHolderMap[viewType] = new Array<sk_sp<ItemViewHolder>>;
        }
        sk_sp<ItemViewHolder>* holder = nullptr;
        for (int i = 0; i < holders->size(); ++i) {
            auto sp = holders->get(i);
            if(!sp->isUsing()){
                holder = &sp;
                break;
            }
        }
        if(holder == nullptr){
            auto sp_holder = sk_ref_sp(adapter->createViewHolder(this, viewType, item));
            holders->add(sp_holder);
            holder = &sp_holder;
        }
        return holder->get();
    }
    const ListAdapter* ListLayout::getAdapter(){
        return adapter.get();
    }
    const Recycler* ListLayout::getRecycler(){
        return recycler.get();
    }
    //------------ holder
    void ItemViewHolder::setLayoutPosition(int pos) {
        mOldPosition = mPosition;
        mPosition = pos;
    }
    bool ItemViewHolder::isSamePosition() {
        return mPosition != NO_POSITION && mPosition == mOldPosition;
    }
    bool ItemViewHolder::isUsing() {
        return used;
    }
    void ItemViewHolder::setUsing(bool u) {
        this->used = u;
    }
}