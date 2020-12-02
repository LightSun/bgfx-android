//
// Created by Administrator on 2020/11/28 0028.
//

#ifndef BGFX_STUDY_EVENTLISTENER_H
#define BGFX_STUDY_EVENTLISTENER_H

#include "lua/SkRefCnt.h"

namespace h7 {
    class Event;

    class EventListener: public SkRefCnt {
    public:
/** Try to handle the given event, if it is applicable.
	 * @return true if the event should be considered {@link Event#handle() handled} by scene2d. */
        virtual bool handle(Event& event){
            return false;
        };
    };
}

#endif //BGFX_STUDY_EVENTLISTENER_H
