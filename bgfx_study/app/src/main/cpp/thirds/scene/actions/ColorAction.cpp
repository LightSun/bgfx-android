//
// Created by Administrator on 2020/12/3 0003.
//
#include "ColorAction.h"
#include "../EventListener.h"
#include "../Event.h"
#include "../Actor.h"
#include "../Stage.h"
#include "../Group.h"

namespace h7 {

    void ColorAction::begin() {
        if (color == nullptr) color = &target->getColor();
        startR = color->r();
        startG = color->g();
        startB = color->b();
        startA = color->a();
    }

    void ColorAction::update(float percent) {
        if (percent == 0)
            color->set(startR, startG, startB, startA);
        else if (percent == 1)
            color->set(*end);
        else {
            float r = startR + (end->r() - startR) * percent;
            float g = startG + (end->g() - startG) * percent;
            float b = startB + (end->b() - startB) * percent;
            float a = startA + (end->a() - startA) * percent;
            color->set(r, g, b, a);
        }
    }
}