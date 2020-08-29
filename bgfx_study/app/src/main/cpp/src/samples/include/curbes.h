//
// Created by Administrator on 2020/8/2 0002.
//

#include "../../base_demo.h"
#include "printer.h"

static const uint64_t s_ptState[]
        {
                UINT64_C(0),
                BGFX_STATE_PT_TRISTRIP,
                BGFX_STATE_PT_LINES,
                BGFX_STATE_PT_LINESTRIP,
                BGFX_STATE_PT_POINTS,
        };

namespace heaven7_Bgfx_demo{
   class CurbesDemo : public BaseDemo{
   public:
       void init() override ;
       int draw();
       void destroy();

   private:
       bgfx::VertexBufferHandle m_vbh;
       bgfx::IndexBufferHandle m_ibh[BX_COUNTOF(s_ptState)];
       bgfx::ProgramHandle m_program;
       int64_t m_timeOffset;
       int32_t m_pt;

       bool m_r;
       bool m_g;
       bool m_b;
       bool m_a;
       Printer printer;
   };
}