//
// Created by Administrator on 2020/12/31 0031.
//

#ifndef BGFX_STUDY_LAYOUTMANAGERS_H
#define BGFX_STUDY_LAYOUTMANAGERS_H

#include "ListLayout.h"

namespace h7{
    class LinearLayoutManager: public LayoutManager{

    public:
        virtual void layoutChildren(ListLayout* layout, float targetX, float targetY, float w, float h);
        virtual void measure(ListLayout* layout, float& w, float& h);

    private:
        bool vertical;
        int offset = -1;
    };
}

#endif //BGFX_STUDY_LAYOUTMANAGERS_H
