//
// Created by Administrator on 2021/1/5 0005.
//

#include "ScrollManager.h"
#include "SceneHelper.h"

namespace h7{


    ScrollManager::ScrollManager(Actor* actor, h7::ScrollManager::Callback *cb): actor(actor), cb(cb) {

    }
    float ScrollManager::getScrollX() const {
        return _scrollInfo.scrollX;
    }
    void ScrollManager::setScrollX(float scrollX) {
        scrollTo(scrollX, _scrollInfo.scrollY);
    }
    float ScrollManager::getScrollY() const {
        return _scrollInfo.scrollY;
    }
    void ScrollManager::setScrollY(float scrollY) {
        scrollTo(_scrollInfo.scrollX, scrollY);
    }
    void ScrollManager::setScrollMode(unsigned char mode) {
        _scrollInfo.mode = mode;
    }
    void ScrollManager::scrollTo(float x, float y) {
        if(_scrollInfo.scrollX != x || _scrollInfo.scrollY != y){
            float dx = x - _scrollInfo.scrollX;
            float dy = y - _scrollInfo.scrollY;
            _scrollInfo.scrollX = x;
            _scrollInfo.scrollY = y;
            //record scroll info
            if(cb->canScrollHorizontal()){
                if(dx > 0){
                    //fingure scroll right. content scroll left
                    _scrollInfo.dir = ScrollInfo::SCROLL_DIRECTION_RIGHT;
                } else if(dx < 0){
                    _scrollInfo.dir = ScrollInfo::SCROLL_DIRECTION_LEFT;
                }
            }
            if(cb->canScrollVertical()){
                if(dy > 0){
                    _scrollInfo.dir = ScrollInfo::SCROLL_DIRECTION_DOWN;
                } else if(dy < 0){
                    _scrollInfo.dir = ScrollInfo::SCROLL_DIRECTION_UP;
                }
            }
            //fire scroll changed
            actor->onScrollChanged(dx, dy);
            ScrollEvent event;
            event.setScrollState(ScrollInfo::SCROLL_STATE_NONE);
            event.setDx(dx);
            event.setDy(dy);
            actor->fire(event);
        }
    }
    void ScrollManager::scrollBy(float dx, float dy) {
        if(dx != 0 || dy != 0){
            scrollTo(_scrollInfo.scrollX + dx, _scrollInfo.scrollY + dy);
        }
    }
    void ScrollManager::setScrollState(unsigned char newState) {
        if(_scrollInfo.scrollState != newState){
            _scrollInfo.scrollState = newState;
            //fire state changed
            ScrollEvent event;
            event.setScrollState(newState);
            event.setDx(0);
            event.setDy(0);
            actor->fire(event);
        }
    }
    void ScrollManager::stopScroll() {
        setScrollState(ScrollInfo::SCROLL_STATE_IDLE);
    }
    unsigned char ScrollManager::getScrollDir() {
        return _scrollInfo.dir;
    }
    unsigned char ScrollManager::getScrollMode() {
        return _scrollInfo.mode;
    }
}