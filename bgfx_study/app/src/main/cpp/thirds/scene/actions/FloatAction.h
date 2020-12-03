//
// Created by Administrator on 2020/12/3 0003.
//

#ifndef BGFX_STUDY_FLOATACTION_H
#define BGFX_STUDY_FLOATACTION_H

#include "TemporalAction.h"

namespace h7 {
    class FloatAction : public TemporalAction {

    private:
        float start, end;
        float value;

        /** Creates a FloatAction that transitions from 0 to 1. */
        FloatAction() {
            start = 0;
            end = 1;
        }
        /** Creates a FloatAction that transitions from start to end. */
        FloatAction(float start, float end) {
            this->start = start;
            this->end = end;
        }
        /** Creates a FloatAction that transitions from start to end. */
        FloatAction(float start, float end, float duration):TemporalAction(duration) {
            this->start = start;
            this->end = end;
        }

        /** Creates a FloatAction that transitions from start to end. */
        FloatAction(float start, float end, float duration,Interpolation* interpolation):TemporalAction(duration, interpolation) {
            this->start = start;
            this->end = end;
        }

        void begin() {
            value = start;
        }
        void update(float percent) {
            if (percent == 0)
                value = start;
            else if (percent == 1)
                value = end;
            else
                value = start + (end - start) * percent;
        }

        /** Gets the current float value. */
        float getValue() {
            return value;
        }
        /** Sets the current float value. */
        void setValue(float value) {
            this->value = value;
        }
        float getStart() {
            return start;
        }
        /** Sets the value to transition from. */
        void setStart(float start) {
            this->start = start;
        }
        float getEnd() {
            return end;
        }
        /** Sets the value to transition to. */
        void setEnd(float end) {
            this->end = end;
        }
    };
}

#endif //BGFX_STUDY_FLOATACTION_H
