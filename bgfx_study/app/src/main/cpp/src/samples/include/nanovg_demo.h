//
// Created by Administrator on 2020/10/31 0031.
//

#ifndef BGFX_STUDY_NANOVG_DEMO_H
#define BGFX_STUDY_NANOVG_DEMO_H

#include "../../base_demo.h"
#include "nanovg/nanovg.h"

namespace heaven7_Bgfx_demo{

    struct DemoData {
        int fontNormal, fontBold, fontIcons, fontEmoji;
        int images[12];
    };

    class NanovgDemo: public BaseDemo{

    public:
        void init() override;
        int draw() override;

        void destroy() override ;

     /*   uint32_t m_width;
        uint32_t m_height;
        uint32_t m_debug;
        uint32_t m_reset;*/

        //entry::MouseState m_mouseState;

        int64_t m_timeOffset;

        NVGcontext* m_nvg;
        DemoData m_data;
    };
}

#endif //BGFX_STUDY_NANOVG_DEMO_H
