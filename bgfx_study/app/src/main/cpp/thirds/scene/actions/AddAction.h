//
// Created by Administrator on 2020/12/3 0003.
//

#ifndef BGFX_STUDY_ADDACTION_H
#define BGFX_STUDY_ADDACTION_H

#include "../Action.h"
#include "../Actor.h"
#include "../../lua/SkRefCnt.h"

namespace h7 {

    class AddAction : public Action {
    private:
        sk_sp<Action> action;

    public:
        bool act(float delta) {
            target->addAction(action.get());
            return true;
        }
        Action *getAction() {
            return action.get();
        }
        void setAction(Action* _ac) {
            action.reset(_ac);
        }
        void restart() {
            if (action != nullptr) {
                action->restart();
            }
        }
        void reset() override {
            Action::reset();
            action = nullptr;
        }

    };
}

#endif //BGFX_STUDY_ADDACTION_H
