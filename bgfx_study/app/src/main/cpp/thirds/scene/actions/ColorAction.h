//
// Created by Administrator on 2020/12/3 0003.
//

#ifndef BGFX_STUDY_COLORACTION_H
#define BGFX_STUDY_COLORACTION_H

#include "TemporalAction.h"
#include "nancanvas/Color.h"

namespace h7 {
    class Color;

    class ColorAction : public TemporalAction {
    private:
        float startR, startG, startB, startA;
        Color* color;
        Color* end;

    protected:
        virtual void begin() override;

        virtual void update(float percent) override;

    public:
        ColorAction(): end(new Color()){};
        ~ColorAction(){
            if(end){
                free(end);
                end = nullptr;
            }
        };
        Color *getColor() {
            return color;
        }
        /** Sets the color to modify. If null (the default), the {@link #getActor() actor's} {@link Actor#getColor() color} will be
 * used. */
        void setColor(Color *c) {
            color = c;
        }
        Color* getEndColor() {
            return end;
        }
        /** Sets the color to transition to. Required. */
        void setEndColor(Color *color){
            end->set(*color);
        }
    };
}

#endif //BGFX_STUDY_COLORACTION_H
