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

        virtual void setLayoutEnabled(bool enabled);

        virtual void doLayout(float ex, float ey, float ew, float eh);

        int getAlign() const;

        virtual void setAlign(int align);

    private:
        void setLayoutEnabled (Group* parent, bool enabled);

    protected:
        int align = Align::left | Align::top; //gravity
    };
}

#endif //BGFX_STUDY_WIDGETGROUP_H
