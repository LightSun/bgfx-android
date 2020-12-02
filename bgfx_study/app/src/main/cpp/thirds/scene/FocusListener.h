//
// Created by Administrator on 2020/12/1 0001.
//

#ifndef BGFX_STUDY_FOCUSLISTENER_H
#define BGFX_STUDY_FOCUSLISTENER_H

#include "Event.h"
#include "EventListener.h"
#include "SceneHelper.h"
#include "lua/SkRefCnt.h"

namespace h7 {

    class Actor;

    class FocusEvent;

    /** Fired when an actor gains or loses keyboard or scroll focus. Can be cancelled to prevent losing or gaining focus.
    * @author Nathan Sweet */
    class FocusEvent : public Event {
    public:
        /** @author Nathan Sweet */
        enum Type {
            keyboard = 0, scroll = 1
        };
    private:
        bool focused;
        Type type;
        sk_sp<Actor> relatedActor;
    public:
        FocusEvent(){
            setEventType(EVENT_FOCUS);
        }
        inline void reset() override {
            Event::reset();
            relatedActor = nullptr;
        }

        inline bool isFocused() {
            return focused;
        }

        inline void setFocused(bool focused) {
            this->focused = focused;
        }

        inline Type getType() {
            return type;
        }

        inline void setType(Type focusType) {
            this->type = focusType;
        }

        /** The actor related to the event. When focus is lost, this is the new actor being focused, or null. When focus is gained,
         * this is the previous actor that was focused, or null. */
        inline Actor *getRelatedActor() {
            return relatedActor.get();
        }

        /** @param relatedActor May be null. */
        inline void setRelatedActor(Actor *relatedActor) {
            this->relatedActor.reset(relatedActor);
        }
    };

    class FocusListener: public EventListener {
    public:
        virtual bool handle(Event &event) {
           // if (!(event instanceof FocusEvent)) return false;
            if(event.getEventType() == EVENT_FOCUS){
                FocusEvent* focusEvent = (FocusEvent*)&event;
                switch (focusEvent->getType()) {
                    case FocusEvent::Type::keyboard:
                        keyboardFocusChanged(*focusEvent, event.getTarget().get(), focusEvent->isFocused());
                        break;
                    case FocusEvent::Type::scroll:
                        scrollFocusChanged(*focusEvent, event.getTarget().get(), focusEvent->isFocused());
                        break;
                }
            }
            return false;
        }
        /** @param actor The event target, which is the actor that emitted the focus event. */
        virtual void keyboardFocusChanged(FocusEvent &event, const Actor *actor, bool focused) {
        }
        /** @param actor The event target, which is the actor that emitted the focus event. */
        virtual void scrollFocusChanged(FocusEvent &event, const Actor *actor, bool focused) {
        }
    };



}

#endif //BGFX_STUDY_FOCUSLISTENER_H
