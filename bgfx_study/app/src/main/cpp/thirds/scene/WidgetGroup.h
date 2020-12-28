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
        WidgetGroup();
        virtual ~WidgetGroup();

        void addActors(int n, ...);
        int getActorType();

        virtual void setNeedLayout(bool need);
        virtual void setNeedMeasure(bool need);

        virtual void doLayout(float ex, float ey, float ew, float eh);

        int getAlign() const;

        virtual void setAlign(int align);

    private:
        void setNeedLayout(Group* parent, bool enabled);
        void setNeedMeasure(Group* parent, bool enabled);

    protected:
        int align = Align::left | Align::top; //gravity
    };
}

#endif //BGFX_STUDY_WIDGETGROUP_H
