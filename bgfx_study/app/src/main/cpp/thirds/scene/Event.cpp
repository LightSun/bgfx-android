//
// Created by Administrator on 2020/11/27 0027.
//

#include "Event.h"
#include "Actor.h"

namespace h7 {

    void Event::reset() {
        sp_stage = NULL;
        sp_targetActor = NULL;
        sp_listenerActor = NULL;
        capture = false;
        bubbles = true;
        handled = false;
        stopped = false;
        cancelled = false;
    }
}