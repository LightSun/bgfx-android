//
// Created by Administrator on 2020/12/3 0003.
//

#include "../Interpolation.h"
#include "../EventListener.h"
#include "../Event.h"
#include "../Actor.h"
#include "../Stage.h"
#include "../Group.h"
#include "TemporalAction.h"

namespace h7{
    bool TemporalAction::act(float delta){
        if (complete) return true;
        if (!began) {
            begin();
            began = true;
        }
        time += delta;
        complete = time >= duration;
        float percent = complete ? 1 : time / duration;
        if (interpolation != nullptr){
            percent = interpolation->apply(percent);
        }
        update(reverse ? 1 - percent : percent);
        if (complete) {
            end();
        }
        return complete;
    }
}