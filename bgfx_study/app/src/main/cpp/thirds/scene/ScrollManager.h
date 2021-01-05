//
// Created by Administrator on 2021/1/5 0005.
//

#ifndef BGFX_STUDY_SCROLLMANAGER_H
#define BGFX_STUDY_SCROLLMANAGER_H

#include "ActorInfo.h"

namespace h7{
    class Actor;

    class ScrollManager{
    public:
        class Callback{
        public:
            virtual bool canScrollVertical(){ return false;}
            virtual bool canScrollHorizontal(){ return false;}
        };
        ScrollManager(Actor* actor, Callback* cb);

        float getScrollX() const;
        void setScrollX(float scrollX);

        float getScrollY() const;
        void setScrollY(float scrollY);
        void setScrollMode(unsigned char mode);

        void scrollBy(float dx, float dy);
        void scrollTo(float x, float y);
        void setScrollState(unsigned char newState);
        void stopScroll();

        unsigned char getScrollMode();
        unsigned char getScrollDir();

    private:
        ScrollInfo _scrollInfo;
        Callback* cb;
        Actor* actor;
    };
}

#endif //BGFX_STUDY_SCROLLMANAGER_H
