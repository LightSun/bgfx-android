//
// Created by Administrator on 2020/8/31 0031.
//

#ifndef BGFX_STUDY_CALLBACK_H
#define BGFX_STUDY_CALLBACK_H

#include "../../base_demo.h"
#include <stdint.h>

namespace heaven7_Bgfx_demo {

    struct BgfxCallback;
    struct BgfxAllocator;

    class CallbackDemo : public BaseDemo {

    public:
        void init();

        int draw();

        void destroy();

    private:
        bgfx::VertexBufferHandle m_vbh;
        bgfx::IndexBufferHandle m_ibh;
        bgfx::ProgramHandle m_program;
        float m_time;
        uint32_t m_frame;

        BgfxCallback*  _callback;
        BgfxAllocator* _allocator;
    };
}
#endif //BGFX_STUDY_CALLBACK_H
