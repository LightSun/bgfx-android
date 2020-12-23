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
            Actor::sizeChanged();
            invalidate();
        }
    public:
        virtual int getActorType(){
            return H7_ACTOR_TYPE | H7_LAYOUT_TYPE;
        }
        virtual void setLayoutEnabled(bool enabled) {
            Layout::setLayoutEnabled(enabled);
            if (enabled) invalidateHierarchy();
        }
        virtual void validate();

        virtual void invalidate() {
            _needsLayout = true;
        }
        virtual void invalidateHierarchy();

        virtual void pack() {
            setSize(getPrefWidth(), getPrefHeight());
            validate();
        }
        /** If this method is overridden, the super method or {@link #validate()} should be called to ensure the widget is laid out. */
        virtual void draw(NanoCanvas::Canvas& canvas, float parentAlpha) {
            validate();
            Actor::draw(canvas, parentAlpha);
        }
        virtual void layout() {

        }
    };
}

#endif //BGFX_STUDY_WIDGET_H
