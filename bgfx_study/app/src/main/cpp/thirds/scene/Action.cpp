//
// Created by Administrator on 2020/11/27 0027.
//

#include "Action.h"
#include "Actor.h"

namespace h7{
    void Action::setActor(sk_sp<Actor> _actor) {
        this->actor = _actor;
        if (target.get() == nullptr){
            setTarget(_actor);
        }
    }
    void Action::setTarget(sk_sp<Actor> _target) {
        this->target = _target;
    }
    void Action::reset () {
        actor = nullptr;
        target = nullptr;
        restart();
    }
}