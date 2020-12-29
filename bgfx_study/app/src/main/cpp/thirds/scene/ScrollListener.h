//
// Created by Administrator on 2020/12/29 0029.
//

#ifndef BGFX_STUDY_SCROLLLISTENER_H
#define BGFX_STUDY_SCROLLLISTENER_H

#include "Event.h"
#include "include/common_types.h"
#include "EventListener.h"

namespace h7{
    //for state change. dx = 0 and dy = 0
    class ScrollEvent: public Event{

    public:
        ScrollEvent() {
            setEventType(EVENT_SCROLL);
        }

        unsigned char getScrollState() const {
            return scrollState;
        }
        void setScrollState(unsigned char scrollState) {
            ScrollEvent::scrollState = scrollState;
        }
        float getDx() const {
            return dx;
        }
        void setDx(float dx) {
            ScrollEvent::dx = dx;
        }
        float getDy() const {
            return dy;
        }
        void setDy(float dy) {
            ScrollEvent::dy = dy;
        }
    private:
         unsigned char scrollState;
         float dx;
         float dy;
    };

    class ScrollListener: public EventListener{
    public:
        virtual bool handle(Event& event){
            if(event.getEventType() == EVENT_SCROLL){
                ScrollEvent* scrollEvent = (ScrollEvent*)&event;
                if(scrollEvent->getDx() == 0 && scrollEvent->getDy() == 0){
                    onScrollStateChanged(*scrollEvent, scrollEvent->getScrollState());
                } else{
                    onScrolled(*scrollEvent, scrollEvent->getDx(), scrollEvent->getDy());
                }
            }
            return false;
        };
        /**
       * Callback method to be invoked when RecyclerView's scroll state changes.
       *
       * @param event The event
       * @param newState     The updated scroll state. One of {@link #SCROLL_STATE_IDLE},
       *                     {@link #SCROLL_STATE_DRAGGING} or {@link #SCROLL_STATE_SETTLING}.
       */
        virtual void onScrollStateChanged(ScrollEvent& event, unsigned char newState){

        }
        /**
        * Callback method to be invoked when the RecyclerView has been scrolled. This will be
        * called after the scroll has completed.
        * <p>
        * This callback will also be called if visible item range changes after a layout
        * calculation. In that case, dx and dy will be 0.
        * @param event The event
        * @param dx The amount of horizontal scroll.
        * @param dy The amount of vertical scroll.
        */
        virtual void onScrolled(ScrollEvent &event, float dx, float dy) {

        }
    };
}

#endif //BGFX_STUDY_SCROLLLISTENER_H
