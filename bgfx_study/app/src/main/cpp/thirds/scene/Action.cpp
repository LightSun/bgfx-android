//
// Created by Administrator on 2020/11/27 0027.
//

#include "Actor.h"
#include "Action.h"

namespace h7{
    void Action::setActor(const Actor* _actor) {
        this->actor = sk_ref_sp(_actor);
        if (target.get() == nullptr){
            setTarget(_actor);
        }
    }
    void Action::setTarget(const Actor* _target) {
        this->target = sk_ref_sp(_target);
    }
    void Action::reset () {
        actor = nullptr;
        target = nullptr;
        restart();
    }
}