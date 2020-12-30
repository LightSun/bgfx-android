//
// Created by Administrator on 2020/12/23 0023.
//

#ifndef BGFX_STUDY_WIDGET_H
#define BGFX_STUDY_WIDGET_H

#include "Actor.h"
#include "Layout.h"

namespace NanoCanvas {
    class Canvas;
}

namespace h7 {

    class Widget :public Actor, public Layout {

    protected:
        virtual void sizeChanged() {
            requestLayoutAndInvalidate();
        }
    public:
        Widget();
        virtual ~Widget();
        int getActorType();

        virtual void doLayout(float ex, float ey, float ew, float eh);

    };
}

#endif //BGFX_STUDY_WIDGET_H
