//
// Created by Administrator on 2020/12/3 0003.
//

#ifndef BGFX_STUDY_DELEGATEACTION_H
#define BGFX_STUDY_DELEGATEACTION_H

#include "../Action.h"
#include "../Actor.h"
#include "../../lua/SkRefCnt.h"

namespace h7 {

    class DelegateAction : Action {

    protected:
        sk_sp<Action> _del_action;

        virtual bool delegate(float delta){
            return false;
        }
    public:
        /** Sets the wrapped action. */
        virtual void setAction(Action* ac) {
            _del_action.reset(ac);
        }
        inline Action* getAction() {
            return _del_action.get();
        }
        inline bool act(float delta) override {
            return delegate(delta);
        }
        inline void restart() {
            if (_del_action != nullptr) _del_action->restart();
        }
        inline void reset() {
            Action::reset();
            _del_action = nullptr;
        }
        virtual void setActor(Actor* actor) {
            if (_del_action != nullptr) _del_action->setActor(actor);
            Action::setActor(actor);
        }
        virtual void setTarget(Actor* target) {
            if (_del_action != nullptr) _del_action->setTarget(target);
            Action::setTarget(target);
        }
    };

}

#endif //BGFX_STUDY_DELEGATEACTION_H
