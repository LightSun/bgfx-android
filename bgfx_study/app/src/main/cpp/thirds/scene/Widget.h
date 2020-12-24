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
            invalidate();
        }
    public:
        virtual int getActorType();

        virtual void setLayoutEnabled(bool enabled);

        virtual void validate();

        virtual void invalidate();

        virtual void invalidateHierarchy();

        virtual void pack();

        /** If this method is overridden, the super method or {@link #validate()} should be called to ensure the widget is laid out. */
        virtual void draw(NanoCanvas::Canvas& canvas, float parentAlpha);

        virtual void layout() {
        }
    };
}

#endif //BGFX_STUDY_WIDGET_H
