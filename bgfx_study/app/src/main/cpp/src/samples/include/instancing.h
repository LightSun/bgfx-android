//
// Created by Administrator on 2020/8/31 0031.
//

#ifndef BGFX_STUDY_INSTANCING_H
#define BGFX_STUDY_INSTANCING_H

#include "../../base_demo.h"
#include <stdint.h>

namespace heaven7_Bgfx_demo {
    class InstancingDemo :public BaseDemo{

    public:
        void init() override ;
        int draw();
        void destroy();

    private:
        bgfx::VertexBufferHandle m_vbh;
        bgfx::IndexBufferHandle  m_ibh;
        bgfx::ProgramHandle m_program;

        int64_t m_timeOffset;
    };
}

#endif //BGFX_STUDY_INSTANCING_H
