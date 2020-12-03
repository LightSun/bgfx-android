//
// Created by Administrator on 2020/12/3 0003.
//

#include "EventAction.h"
#include "../Actor.h"

namespace h7{

    void EventAction::setTarget(h7::Actor *newTarget) {
        if (target != nullptr) target->removeListener(listener);
        Action::setTarget(newTarget);
        if (newTarget != nullptr) newTarget->addListener(listener);
    }

    EventAction::InternalListener::InternalListener(EventAction *action) : action(action) {}

}