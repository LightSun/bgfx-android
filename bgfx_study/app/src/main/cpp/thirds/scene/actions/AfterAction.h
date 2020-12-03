//
// Created by Administrator on 2020/12/3 0003.
//

#ifndef BGFX_STUDY_AFTERACTION_H
#define BGFX_STUDY_AFTERACTION_H

#include "../Action.h"
#include "../Actor.h"
#include "../../lua/SkRefCnt.h"
#include "../../utils/Array.h"
#include "DelegateAction.h"

namespace h7 {

    class AfterAction : DelegateAction {
    private:
        Array<sk_sp<Action>> waitForActions = Array<sk_sp<Action>>(4);

    public:

        void setTarget(Actor *target) {
            if (target != nullptr) {
                waitForActions.addAll(reinterpret_cast<Array<sk_sp<Action>>*>(&target->getActions()));
            }
            Action::setTarget(target);
        }

        void restart() {
            Action::restart();
            waitForActions.clear();
        }

        bool delegate(float delta) {
            Array<sk_sp<Action>> currentActions = reinterpret_cast<Array<sk_sp<Action>>&>(target->getActions());
            if (currentActions.size() == 1) waitForActions.clear();
            for (int i = waitForActions.size() - 1; i >= 0; i--) {
                sk_sp<Action> &sp = waitForActions.get(i);
                int index = currentActions.indexOf(sp);
                if (index == -1) waitForActions.removeAt(i);
            }
            if (waitForActions.size() > 0) return false;
            return _del_action->act(delta);
        }
    };
}

#endif //BGFX_STUDY_AFTERACTION_H
