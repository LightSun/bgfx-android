//
// Created by Administrator on 2020/11/27 0027.
//

#ifndef BGFX_STUDY_EVENT_H
#define BGFX_STUDY_EVENT_H

#include "lua/SkRefCnt.h"
#include "utils/Pool.h"

#define EVENT_RAW static_cast<unsigned char>(0)
#define EVENT_FOCUS static_cast<unsigned char>(1)
#define EVENT_SCROLL static_cast<unsigned char>(2)

namespace h7 {

    class Stage;

    class Actor;

    class Event : public Poolable {
    private:
        sk_sp<Stage> sp_stage;
        sk_sp<Actor> sp_targetActor;   //the actor who fire event.
        sk_sp<Actor> sp_listenerActor; //the actor who notify event
        bool capture; // true means event occurred during the capture phase
        bool bubbles = true; // true means propagate to target's parents
        bool handled; // true means the event was handled (the stage will eat the input)
        bool stopped; // true means event propagation was stopped
        bool cancelled; // true means propagation was stopped and any action that this event would cause should not happen
        unsigned char _eventType = EVENT_RAW;

    public:
        unsigned char getEventType() const {
            return _eventType;
        }
        void setEventType(unsigned char eventType) {
            _eventType = eventType;
        }
        /** Marks this event as handled. This does not affect event propagation inside scene2d, but causes the {@link Stage}
	 * {@link InputProcessor} methods to return true, which will eat the event so it is not passed on to the application under the
	 * stage. */
        inline void handle() {
            handled = true;
        }
        /** Marks this event cancelled. This {@link #handle() handles} the event and {@link #stop() stops} the event propagation. It
         * also cancels any default action that would have been taken by the code that fired the event. Eg, if the event is for a
         * checkbox being checked, cancelling the event could uncheck the checkbox. */
        inline void cancel() {
            cancelled = true;
            stopped = true;
            handled = true;
        }
        /** Marks this event has being stopped. This halts event propagation. Any other listeners on the {@link #getListenerActor()
	 * listener actor} are notified, but after that no other listeners are notified. */
        inline void stop() {
            stopped = true;
        }
        virtual void reset();

        /** Returns the actor that the event originated from. */
        inline sk_sp<Actor> getTarget () {
            return sp_targetActor;
        }

        inline void setTarget (Actor* targetActor) {
            sp_targetActor.reset(targetActor);
        }
        inline void setTarget (sk_sp<Actor>& targetActor) {
            sp_targetActor = targetActor;
        }

        /** Returns the actor that this listener is attached to. */
        inline sk_sp<Actor> getListenerActor () {
            return sp_listenerActor;
        }

        inline void setListenerActor (Actor* listenerActor) {
            this->sp_listenerActor.reset(listenerActor);
        }
        inline void setListenerActor(sk_sp<Actor>& listenerActor) {
            this->sp_listenerActor = listenerActor;
        }

        inline bool getBubbles () {
            return bubbles;
        }
        /** If true, after the event is fired on the target actor, it will also be fired on each of the parent actors, all the way to
         * the root. */
        inline void setBubbles (bool bubbles) {
            this->bubbles = bubbles;
        }
        /** {@link #handle()} */
        inline bool isHandled () {
            return handled;
        }
        /** @see #stop() */
        inline bool isStopped () {
            return stopped;
        }
        /** @see #cancel() */
        inline bool isCancelled () {
            return cancelled;
        }
        inline void setCapture (bool capture) {
            this->capture = capture;
        }
        /** If true, the event was fired during the capture phase.
         * @see Actor#fire(Event) */
        inline bool isCapture () {
            return capture;
        }
        inline void setStage (Stage* stage) {
            this->sp_stage.reset(stage);
        }
        inline void setStage (sk_sp<Stage>& sp) {
            this->sp_stage = sp;
        }
        /** The stage for the actor the event was fired on. */
        inline sk_sp<Stage> getStage () {
            return sp_stage;
        }
    };
}

#endif //BGFX_STUDY_EVENT_H
