//
// Created by Administrator on 2020/8/31 0031.
//

#ifndef BGFX_STUDY_BUMP_H
#define BGFX_STUDY_BUMP_H

#include "../../base_demo.h"
#include <stdint.h>

namespace heaven7_Bgfx_demo {
    class BumpDemo :public BaseDemo{

    public:
        void init() override ;
        int draw();
        void destroy();

    private:
        bgfx::VertexBufferHandle m_vbh;
        bgfx::IndexBufferHandle  m_ibh;
        bgfx::UniformHandle s_texColor;
        bgfx::UniformHandle s_texNormal;
        bgfx::UniformHandle u_lightPosRadius;
        bgfx::UniformHandle u_lightRgbInnerR;
        bgfx::ProgramHandle m_program;
        bgfx::TextureHandle m_textureColor;
        bgfx::TextureHandle m_textureNormal;
        uint16_t m_numLights;
        bool m_instancingSupported;

        int64_t m_timeOffset;
    };
}

#endif //BGFX_STUDY_INSTANCING_H
