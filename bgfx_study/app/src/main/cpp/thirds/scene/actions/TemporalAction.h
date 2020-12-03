//
// Created by Administrator on 2020/12/3 0003.
//

#ifndef BGFX_STUDY_TEMPORALACTION_H
#define BGFX_STUDY_TEMPORALACTION_H

#include "../Action.h"
//#include "../Interpolation.h"

namespace h7 {
    class Interpolation;

    class TemporalAction: public Action {
    public:
        float duration, time;
        Interpolation *interpolation;
        bool reverse, began, complete;

        TemporalAction() {
        }

        TemporalAction(float duration) {
            this->duration = duration;
        }

        TemporalAction(float duration, Interpolation *interpolation) {
            this->duration = duration;
            this->interpolation = interpolation;
        }

        virtual bool act(float delta);

    protected:
        /** Called the first time {@link #act(float)} is called. This is a good place to query the {@link #actor actor's} starting
         * state. */
        virtual void begin() {
        }

        /** Called the last time {@link #act(float)} is called. */
        virtual void end() {
        }

        /** Called each frame.
         * @param percent The percentage of completion for this action, growing from 0 to 1 over the duration. If
         *           {@link #setReverse(bool) reversed}, this will shrink from 1 to 0. */
        virtual void update(float percent) {
        }

    public:
        /** Skips to the end of the transition. */
        void finish() {
            time = duration;
        }

        void restart() {
            time = 0;
            began = false;
            complete = false;
        }

        void reset() {
            Action::reset();
            reverse = false;
            interpolation = nullptr;
        }

        /** Gets the transition time so far. */
        float getTime() {
            return time;
        }

        /** Sets the transition time so far. */
        void setTime(float time) {
            this->time = time;
        }

        float getDuration() {
            return duration;
        }

        /** Sets the length of the transition in seconds. */
        void setDuration(float duration) {
            this->duration = duration;
        }
        inline Interpolation* getInterpolation() {
            return interpolation;
        }
        virtual void setInterpolation(Interpolation* interpolation) {
            this->interpolation = interpolation;
        }
        bool isReverse() {
            return reverse;
        }
        /** When true, the action's progress will go from 100% to 0%. */
        void setReverse(bool reverse) {
            this->reverse = reverse;
        }
        /** Returns true after {@link #act(float)} has been called where time >= duration. */
        bool isComplete() {
            return complete;
        }

    };
}

#endif //BGFX_STUDY_TEMPORALACTION_H
