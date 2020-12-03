//
// Created by Administrator on 2020/12/3 0003.
//

#ifndef BGFX_STUDY_FLOATACTION_H
#define BGFX_STUDY_FLOATACTION_H

#include "TemporalAction.h"

namespace h7 {
    class IntAction : public TemporalAction {

    private:
        int start, end;
        int value;

        /** Creates a FloatAction that transitions from 0 to 1. */
        IntAction() {
            start = 0;
            end = 1;
        }
        /** Creates a FloatAction that transitions from start to end. */
        IntAction(int start, int end) {
            this->start = start;
            this->end = end;
        }
        /** Creates a FloatAction that transitions from start to end. */
        IntAction(int start, int end, float duration):TemporalAction(duration) {
            this->start = start;
            this->end = end;
        }
        /** Creates a FloatAction that transitions from start to end. */
        IntAction(int start, int end, float duration,Interpolation* interpolation):TemporalAction(duration, interpolation) {
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
                value = static_cast<int>(start + (end - start) * percent);
        }

        /** Gets the current float value. */
        int getValue() {
            return value;
        }
        /** Sets the current float value. */
        void setValue(int value) {
            this->value = value;
        }
        int getStart() {
            return start;
        }
        /** Sets the value to transition from. */
        void setStart(int start) {
            this->start = start;
        }
        int getEnd() {
            return end;
        }
        /** Sets the value to transition to. */
        void setEnd(int end) {
            this->end = end;
        }
    };
}

#endif //BGFX_STUDY_FLOATACTION_H
