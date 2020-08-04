//
// Created by Administrator on 2020/8/2 0002.
//

#include "include/curbes.h"
#include "bgfx_utils.h"
#include "bx/timer.h"

struct PosColorVertex
{
    float m_x;
    float m_y;
    float m_z;
    uint32_t m_abgr;

    static void init()
    {
        ms_layout
                .begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
                .end();
    };

    static bgfx::VertexLayout ms_layout;
};

bgfx::VertexLayout PosColorVertex::ms_layout;

static PosColorVertex s_cubeVertices[] =
        {
                {-1.0f,  1.0f,  1.0f, 0xff000000 },
                { 1.0f,  1.0f,  1.0f, 0xff0000ff },
                {-1.0f, -1.0f,  1.0f, 0xff00ff00 },
                { 1.0f, -1.0f,  1.0f, 0xff00ffff },
                {-1.0f,  1.0f, -1.0f, 0xffff0000 },
                { 1.0f,  1.0f, -1.0f, 0xffff00ff },
                {-1.0f, -1.0f, -1.0f, 0xffffff00 },
                { 1.0f, -1.0f, -1.0f, 0xffffffff },
        };

static const uint16_t s_cubeTriList[] =
        {
                0, 1, 2, // 0
                1, 3, 2,
                4, 6, 5, // 2
                5, 6, 7,
                0, 2, 4, // 4
                4, 2, 6,
                1, 5, 3, // 6
                5, 7, 3,
                0, 4, 1, // 8
                4, 5, 1,
                2, 3, 6, // 10
                6, 3, 7,
        };

static const uint16_t s_cubeTriStrip[] =
        {
                0, 1, 2,
                3,
                7,
                1,
                5,
                0,
                4,
                2,
                6,
                7,
                4,
                5,
        };

static const uint16_t s_cubeLineList[] =
        {
                0, 1,
                0, 2,
                0, 4,
                1, 3,
                1, 5,
                2, 3,
                2, 6,
                3, 7,
                4, 5,
                4, 6,
                5, 7,
                6, 7,
        };

static const uint16_t s_cubeLineStrip[] =
        {
                0, 2, 3, 1, 5, 7, 6, 4,
                0, 2, 6, 4, 5, 7, 3, 1,
                0,
        };

static const uint16_t s_cubePoints[] =
        {
                0, 1, 2, 3, 4, 5, 6, 7
        };

static const char* s_ptNames[]
        {
                "Triangle List",
                "Triangle Strip",
                "Lines",
                "Line Strip",
                "Points",
        };
BX_STATIC_ASSERT(BX_COUNTOF(s_ptState) == BX_COUNTOF(s_ptNames) );

namespace heaven7_Bgfx_demo{

    void CurbesDemo::init(InitConfig *config) {
        m_debug  = BGFX_DEBUG_NONE;
        m_reset  = BGFX_RESET_VSYNC;
        BaseDemo::init(config);

        // Create vertex stream declaration.
        PosColorVertex::init();

        // Create static vertex buffer.
        m_vbh = bgfx::createVertexBuffer(
                // Static data can be passed with bgfx::makeRef
                bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices) )
                , PosColorVertex::ms_layout
        );

        // Create static index buffer for triangle list rendering.
        m_ibh[0] = bgfx::createIndexBuffer(
                // Static data can be passed with bgfx::makeRef
                bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList) )
        );

        // Create static index buffer for triangle strip rendering.
        m_ibh[1] = bgfx::createIndexBuffer(
                // Static data can be passed with bgfx::makeRef
                bgfx::makeRef(s_cubeTriStrip, sizeof(s_cubeTriStrip) )
        );

        // Create static index buffer for line list rendering.
        m_ibh[2] = bgfx::createIndexBuffer(
                // Static data can be passed with bgfx::makeRef
                bgfx::makeRef(s_cubeLineList, sizeof(s_cubeLineList) )
        );

        // Create static index buffer for line strip rendering.
        m_ibh[3] = bgfx::createIndexBuffer(
                // Static data can be passed with bgfx::makeRef
                bgfx::makeRef(s_cubeLineStrip, sizeof(s_cubeLineStrip) )
        );

        // Create static index buffer for point list rendering.
        m_ibh[4] = bgfx::createIndexBuffer(
                // Static data can be passed with bgfx::makeRef
                bgfx::makeRef(s_cubePoints, sizeof(s_cubePoints) )
        );

        // Create program from shaders.
        m_program = loadProgram("vs_cubes", "fs_cubes");

        m_timeOffset = bx::getHPCounter();
    }
    void CurbesDemo::draw() {
        int m_width = config.win_width;
        int m_height = config.win_height;
        float time = (float)( (bx::getHPCounter()-m_timeOffset)/double(bx::getHPFrequency() ) );

        const bx::Vec3 at  = { 0.0f, 0.0f,   0.0f };
        const bx::Vec3 eye = { 0.0f, 0.0f, -35.0f };

        // Set view and projection matrix for view 0.
        {
            float view[16];
            bx::mtxLookAt(view, eye, at);

            float proj[16];
            bx::mtxProj(proj, 60.0f, float(m_width)/float(m_height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
            bgfx::setViewTransform(0, view, proj);

            // Set view 0 default viewport.
            bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height) );
        }

        // This dummy draw call is here to make sure that view 0 is cleared
        // if no other draw calls are submitted to view 0.
        bgfx::touch(0);

        bgfx::IndexBufferHandle ibh = m_ibh[m_pt];
        uint64_t state = 0
                         | (m_r ? BGFX_STATE_WRITE_R : 0)
                         | (m_g ? BGFX_STATE_WRITE_G : 0)
                         | (m_b ? BGFX_STATE_WRITE_B : 0)
                         | (m_a ? BGFX_STATE_WRITE_A : 0)
                         | BGFX_STATE_WRITE_Z
                         | BGFX_STATE_DEPTH_TEST_LESS
                         | BGFX_STATE_CULL_CW
                         | BGFX_STATE_MSAA
                         | s_ptState[m_pt]
        ;

        // Submit 11x11 cubes.
        for (uint32_t yy = 0; yy < 11; ++yy)
        {
            for (uint32_t xx = 0; xx < 11; ++xx)
            {
                float mtx[16];
                bx::mtxRotateXY(mtx, time + xx*0.21f, time + yy*0.37f);
                mtx[12] = -15.0f + float(xx)*3.0f;
                mtx[13] = -15.0f + float(yy)*3.0f;
                mtx[14] = 0.0f;

                // Set model matrix for rendering.
                bgfx::setTransform(mtx);

                // Set vertex and index buffer.
                bgfx::setVertexBuffer(0, m_vbh);
                bgfx::setIndexBuffer(ibh);

                // Set render states.
                bgfx::setState(state);

                // Submit primitive for rendering to view 0.
                bgfx::submit(0, m_program);
            }
        }

        // Advance to next frame. Rendering thread will be kicked to
        // process submitted rendering primitives.
        bgfx::frame();
    }
    void CurbesDemo::destroy() {
// Cleanup.
        for (uint32_t ii = 0; ii < BX_COUNTOF(m_ibh); ++ii)
        {
            bgfx::destroy(m_ibh[ii]);
        }

        bgfx::destroy(m_vbh);
        bgfx::destroy(m_program);

        BaseDemo::destroy();
    }

}