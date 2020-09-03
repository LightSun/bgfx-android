//
// Created by Administrator on 2020/9/3 0003.
//

#ifndef BGFX_STUDY_VECTORDISPLAY_23_H
#define BGFX_STUDY_VECTORDISPLAY_23_H

#include "../../base_demo.h"
#include "../vectordisplay.h"

namespace heaven7_Bgfx_demo {

    class VectorDisplayDemo : public BaseDemo {
    public:
        void init() override;
        int draw() override;
        void destroy() override ;

    private:
        VectorDisplay m_vd;

        uint32_t m_oldWidth;
        uint32_t m_oldHeight;
    };
}

#endif //BGFX_STUDY_VECTORDISPLAY_23_H
