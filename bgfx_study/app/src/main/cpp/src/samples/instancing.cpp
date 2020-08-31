//
// Created by Administrator on 2020/8/31 0031.
//

#include <bgfx_utils.h>
#include <bx/timer.h>
#include "include/instancing.h"

namespace heaven7_Bgfx_demo {
    struct PosColorVertex2 {
        float m_x;
        float m_y;
        float m_z;
        uint32_t m_abgr;

        static void init() {
            ms_layout
                    .begin()
                    .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                    .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
                    .end();
        };

        static bgfx::VertexLayout ms_layout;
    };

    bgfx::VertexLayout PosColorVertex2::ms_layout;

    static PosColorVertex2 s_cubeVertices[8] =
            {
                    {-1.0f, 1.0f,  1.0f,  0xff000000},
                    {1.0f,  1.0f,  1.0f,  0xff0000ff},
                    {-1.0f, -1.0f, 1.0f,  0xff00ff00},
                    {1.0f,  -1.0f, 1.0f,  0xff00ffff},
                    {-1.0f, 1.0f,  -1.0f, 0xffff0000},
                    {1.0f,  1.0f,  -1.0f, 0xffff00ff},
                    {-1.0f, -1.0f, -1.0f, 0xffffff00},
                    {1.0f,  -1.0f, -1.0f, 0xffffffff},
            };

    static const uint16_t s_cubeIndices[36] =
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

    void InstancingDemo::init() {
        m_debug = BGFX_DEBUG_TEXT;
        m_reset = BGFX_RESET_VSYNC;

        BaseDemo::init();

        // Create vertex stream declaration.
        PosColorVertex2::init();

        // Create static vertex buffer.
        m_vbh = bgfx::createVertexBuffer(
                bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)), PosColorVertex2::ms_layout
        );

        // Create static index buffer.
        m_ibh = bgfx::createIndexBuffer(
                bgfx::makeRef(s_cubeIndices, sizeof(s_cubeIndices))
        );

        // Create program from shaders.
        m_program = loadProgram("vs_instancing", "fs_instancing");

        m_timeOffset = bx::getHPCounter();
    }

    void InstancingDemo::destroy() {
        bgfx::destroy(m_ibh);
        bgfx::destroy(m_vbh);
        bgfx::destroy(m_program);

        BaseDemo::destroy();
    }

    int InstancingDemo::draw() {
        int m_width = config.win_width;
        int m_height = config.win_height;
// Set view 0 default viewport.
        bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));

        // This dummy draw call is here to make sure that view 0 is cleared
        // if no other draw calls are submitted to view 0.
        bgfx::touch(0);

        float time = (float) ((bx::getHPCounter() - m_timeOffset) / double(bx::getHPFrequency()));

        // Get renderer capabilities info.
        const bgfx::Caps *caps = bgfx::getCaps();

        // Check if instancing is supported.
        if (0 == (BGFX_CAPS_INSTANCING & caps->supported)) {
            // When instancing is not supported by GPU, implement alternative
            // code path that doesn't use instancing.
            bool blink = uint32_t(time * 3.0f) & 1;
            bgfx::dbgTextPrintf(0, 0, blink ? 0x4f : 0x04, " Instancing is not supported by GPU. ");
        } else {
            const bx::Vec3 at = {0.0f, 0.0f, 0.0f};
            const bx::Vec3 eye = {0.0f, 0.0f, -35.0f};

            // Set view and projection matrix for view 0.
            {
                float view[16];
                bx::mtxLookAt(view, eye, at);

                float proj[16];
                bx::mtxProj(proj, 60.0f, float(m_width) / float(m_height), 0.1f, 100.0f,
                            bgfx::getCaps()->homogeneousDepth);
                bgfx::setViewTransform(0, view, proj);

                // Set view 0 default viewport.
                bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));
            }

            // 80 bytes stride = 64 bytes for 4x4 matrix + 16 bytes for RGBA color.
            const uint16_t instanceStride = 80;
            // 11x11 cubes
            const uint32_t numInstances = 121;

            if (numInstances == bgfx::getAvailInstanceDataBuffer(numInstances, instanceStride)) {
                bgfx::InstanceDataBuffer idb;
                bgfx::allocInstanceDataBuffer(&idb, numInstances, instanceStride);

                uint8_t *data = idb.data;

                // Write instance data for 11x11 cubes.
                for (uint32_t yy = 0; yy < 11; ++yy) {
                    for (uint32_t xx = 0; xx < 11; ++xx) {
                        float *mtx = (float *) data;
                        bx::mtxRotateXY(mtx, time + xx * 0.21f, time + yy * 0.37f);
                        mtx[12] = -15.0f + float(xx) * 3.0f;
                        mtx[13] = -15.0f + float(yy) * 3.0f;
                        mtx[14] = 0.0f;

                        float *color = (float *) &data[64];
                        color[0] = bx::sin(time + float(xx) / 11.0f) * 0.5f + 0.5f;
                        color[1] = bx::cos(time + float(yy) / 11.0f) * 0.5f + 0.5f;
                        color[2] = bx::sin(time * 3.0f) * 0.5f + 0.5f;
                        color[3] = 1.0f;

                        data += instanceStride;
                    }
                }

                // Set vertex and index buffer.
                bgfx::setVertexBuffer(0, m_vbh);
                bgfx::setIndexBuffer(m_ibh);

                // Set instance data buffer.
                bgfx::setInstanceDataBuffer(&idb);

                // Set render states.
                bgfx::setState(BGFX_STATE_DEFAULT);

                // Submit primitive for rendering to view 0.
                bgfx::submit(0, m_program);
            }
        }

        // Advance to next frame. Rendering thread will be kicked to
        // process submitted rendering primitives.
        bgfx::frame();
        return 0;
    }
}