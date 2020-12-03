//
// Created by Administrator on 2020/12/3 0003.
//

#include "AlphaAction.h"
#include "../EventListener.h"
#include "../Event.h"
#include "../Actor.h"
#include "../Stage.h"
#include "../Group.h"

namespace h7 {

    void AlphaAction::begin() {
        if(color == nullptr){
            color = &target->getColor();
        }
        start = color->a();
    }

    void AlphaAction::update(float percent) {
        if (percent == 0)
            color->a(start);
        else if (percent == 1)
            color->a(end);
        else
            color->a(start + (end - start) * percent);
    }
}
