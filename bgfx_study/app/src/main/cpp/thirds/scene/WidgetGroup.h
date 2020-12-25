//
// Created by Administrator on 2020/12/23 0023.
//

#ifndef BGFX_STUDY_WIDGETGROUP_H
#define BGFX_STUDY_WIDGETGROUP_H

#include "Group.h"
#include "Layout.h"

namespace h7{
    class WidgetGroup: public Group, public Layout{

    public:
        void addActors(int n, ...);
        int getActorType();

        virtual void setLayoutEnabled (bool enabled);

        virtual void validate();

        virtual void invalidateHierarchy();

        virtual void childrenChanged ();
        virtual void sizeChanged ();
        virtual void pack ();

        /** If this method is overridden, the super method or {@link #validate()} should be called to ensure the widget is laid out. */
        virtual void draw(NanoCanvas::Canvas& canvas, float parentAlpha);
    private:
        void setLayoutEnabled (Group* parent, bool enabled);
    };
}

#endif //BGFX_STUDY_WIDGETGROUP_H
