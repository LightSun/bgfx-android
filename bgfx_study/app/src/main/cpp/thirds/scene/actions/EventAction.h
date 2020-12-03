//
// Created by Administrator on 2020/12/3 0003.
//

#ifndef BGFX_STUDY_EVENTACTION_H
#define BGFX_STUDY_EVENTACTION_H

#include "../Action.h"
#include "../Stage.h"
#include "../Group.h"
#include "../Event.h"
#include "../EventListener.h"

namespace h7 {

    class EventAction : public Action{
    private:
        int etype;
        bool result, active;

        class InternalListener: public EventListener{
        public:
            EventAction* action;
            InternalListener(EventAction* action);
            inline bool handle(Event& event) {
                if(!action->active || event.getEventType() != action->etype){
                    return false;
                }
                action->result = action->onHandle(event);
                return action->result;
            };
        };
        InternalListener* listener;
    public:
        EventAction(int etype) : etype(etype), listener(new InternalListener(this)) {
        }
        ~EventAction(){
            if(listener){
                listener->unref();
                listener = nullptr;
            }
        }
        inline void restart() {
            result = false;
            active = false;
        }
        virtual void setTarget(Actor *newTarget);

        inline bool act(float delta) {
            if(!active){
                active = true;
            }
            return result;
        }
        inline bool isActive() {
            return active;
        }
        inline void setActive(bool active) {
            this->active = active;
        }

    protected:
        /** Called when the specific type of event occurs on the actor.
       * @return true if the event should be considered {@link Event#handle() handled} and this EventAction considered complete. */
        virtual bool onHandle(Event& event){
            return false;
        }
    };
}

#endif //BGFX_STUDY_EVENTACTION_H
