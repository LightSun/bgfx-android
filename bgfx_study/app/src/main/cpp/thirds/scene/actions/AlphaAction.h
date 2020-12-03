//
// Created by Administrator on 2020/12/3 0003.
//

#ifndef BGFX_STUDY_ALPHAACTION_H
#define BGFX_STUDY_ALPHAACTION_H

#include <nancanvas/Color.h>
#include "TemporalAction.h"

namespace h7 {

    class AlphaAction : public TemporalAction {
    private:
        float start, end;
        Color* color;

    protected:
        virtual void begin();

        virtual void update(float percent);

    public:
        inline Color* getColor(){
            return color;
        }
        /** Sets the color to modify. If null (the default), the {@link #getActor() actor's} {@link Actor#getColor() color} will be
         * used. */
        inline void setColor(Color* c){
            color = c;
        }
        inline float getAlpha() {
            return end;
        }
        inline void setAlpha(float alpha) {
            this->end = alpha;
        }
    };
}

#endif //BGFX_STUDY_ALPHAACTION_H
