//
// Created by Administrator on 2020/12/25 0025.
//

#ifndef BGFX_STUDY_LINEARLAYOUT_H
#define BGFX_STUDY_LINEARLAYOUT_H

#include "../WidgetGroup.h"
#include "../../lua/SkRefCnt.h"

namespace h7{
    typedef struct LinearLayoutParams: public LayoutParams {
        int weight;
        LinearLayoutParams();
    }LinearLayoutParams;

    class LinearLayout: public WidgetGroup{

    public:
        virtual void onLayoutChildren(float targetX, float targetY, float w, float h);

        bool isVertical() const;
        void setVertical(bool vertical) ;

    private:
        bool vertical = true;
    };
}

#endif //BGFX_STUDY_LINEARLAYOUT_H
