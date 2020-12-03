//
// Created by Administrator on 2020/12/3 0003.
//

#ifndef BGFX_STUDY_ADDLISTENERACTION_H
#define BGFX_STUDY_ADDLISTENERACTION_H

#include "../Action.h"
#include "../Actor.h"
#include "../EventListener.h"
#include "../../lua/SkRefCnt.h"

namespace h7 {

    class AddListenerAction : Action {
    private:
        sk_sp<EventListener> listener;
        bool capture;
    public:

        bool act(float delta) {
            if (capture)
                target->addCaptureListener(listener.get());
            else
                target->addListener(listener.get());
            return true;
        }
        EventListener* getListener() {
            return listener.get();
        }
        void setListener(EventListener* l) {
            listener.reset(l);
        }
        bool getCapture() {
            return capture;
        }
        void setCapture(bool _capture) {
            this->capture = _capture;
        }
        void reset() {
            Action::reset();
            listener = nullptr;
        }
    };
}

#endif //BGFX_STUDY_ADDLISTENERACTION_H
