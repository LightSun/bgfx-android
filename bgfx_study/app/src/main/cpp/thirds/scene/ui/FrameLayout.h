//
// Created by Administrator on 2020/12/25 0025.
//

#ifndef BGFX_STUDY_FRAMELAYOUT_H
#define BGFX_STUDY_FRAMELAYOUT_H

#include "../WidgetGroup.h"

namespace h7{

    class FrameLayout: public WidgetGroup{

    public:

        virtual void measure(float restrictW, float restrictH, float &outW, float &outH);

        virtual void onLayoutChildren(float targetX, float targetY, float w, float h);

    };
}

#endif //BGFX_STUDY_FRAMELAYOUT_H
