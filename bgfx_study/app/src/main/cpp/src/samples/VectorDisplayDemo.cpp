//
// Created by Administrator on 2020/9/3 0003.
//

#include "include/vectordisplay_23.h"
#include "vectordisplay.h"


namespace heaven7_Bgfx_demo{

    struct PosColorVertex3
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
                    .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
                    .end();
        }

        static bgfx::VertexLayout ms_layout;
    };

    bgfx::VertexLayout PosColorVertex3::ms_layout;

    void VectorDisplayDemo::init() {
        int m_width = config.win_width;
        int m_height = config.win_height;
        m_debug = BGFX_DEBUG_NONE;
        m_reset = BGFX_RESET_VSYNC;
        BaseDemo::init();
        //set up VectorDisplay
        const bgfx::RendererType::Enum renderer = bgfx::getRendererType();
        float texelHalf = bgfx::RendererType::Direct3D9 == renderer ? 0.5f : 0.0f;
        bool originBottomLeft = bgfx::RendererType::OpenGL == renderer
                                || bgfx::RendererType::OpenGLES == renderer;
        m_vd.init(originBottomLeft, texelHalf);
        m_vd.setup(uint16_t(m_width), uint16_t(m_height) );

        // Create vertex stream declaration.
        PosColorVertex3::init();

        m_oldWidth = m_width;
        m_oldHeight = m_height;
    }
    void VectorDisplayDemo::destroy() {
        // Cleanup.
        m_vd.teardown();
        BaseDemo::destroy();
    }
    int VectorDisplayDemo::draw() {
        int m_width = config.win_width;
        int m_height = config.win_height;

        if (m_oldWidth  != m_width
            ||  m_oldHeight != m_height)
        {
            m_oldWidth  = m_width;
            m_oldHeight = m_height;
            m_vd.resize(uint16_t(m_width), uint16_t(m_height) );
        }

        const bx::Vec3 at  = { 0.0f, 0.0f,   0.0f };
        const bx::Vec3 eye = { 0.0f, 0.0f, -35.0f };

        float view[16];
        float proj[16];
        bx::mtxLookAt(view, eye, at);
        bx::mtxProj(proj, 60.0f, float(m_width) / float(m_height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
        // Set view and projection matrix for view 0.
        bgfx::setViewTransform(0, view, proj);

        // Set view 0 default viewport.
        bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height) );

        // This dummy draw call is here to make sure that view 0 is cleared
        // if no other draw calls are submitted to view 0.
        bgfx::touch(0);

        m_vd.beginFrame();

        //simplex test
        m_vd.setDrawColor(0.7f, 0.7f, 1.0f);
        m_vd.drawSimplexFont(50.0f,  80.0f, 1.5f, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        m_vd.drawSimplexFont(50.0f, 140.0f, 1.5f, "abcdefghijklmnopqrstuvwxyz");
        m_vd.drawSimplexFont(50.0f, 200.0f, 1.5f, "!@#$%^&*()-=<>/?;:'\"{}[]|\\+=-_");

        m_vd.setDrawColor(1.0f, 0.7f, 0.7f);

        //test pattern for lines
        for (int ii = 0; ii < 4; ii++)
        {
            for (int jj = 0; jj < ii; jj++) //draw more intensive lines
            {
                m_vd.drawLine(50.0f, 350.0f + 40 * ii, 200.0f, 350.0f + 40 * ii);
            }
        }

        for (int ii = 0; ii < 4; ii++)
        {
            for (int jj = 0; jj <= ii; jj++)
            {
                m_vd.drawLine(50.0f + 40 * ii, 600.0f, 50.0f + 40 * ii, 700.0f);
            }
        }

        //
        // test pattern for shapes
        //
        m_vd.setDrawColor(0.7f, 0.7f, 1.0f);
        m_vd.drawCircle(250.0f, 450.0f, 10.0f, 32.0f);
        m_vd.drawCircle(300.0f, 450.0f, 30.0f, 32.0f);
        m_vd.drawCircle(400.0f, 450.0f, 60.0f, 32.0f);
        m_vd.drawCircle(500.0f, 450.0f, 80.0f, 64.0f);

        m_vd.setDrawColor(0.7f, 1.0f, 0.7f);
        m_vd.drawBox(250.0f, 600.0f, 10.0f, 10.0f);
        m_vd.drawBox(300.0f, 600.0f, 30.0f, 30.0f);
        m_vd.drawBox(350.0f, 600.0f, 60.0f, 60.0f);
        m_vd.drawBox(450.0f, 600.0f, 80.0f, 80.0f);

        m_vd.setDrawColor(1.0f, 0.7f, 1.0f);
        m_vd.drawWheel(bx::kPi, 800.0f, 450.0f, 80.0f);
        m_vd.drawWheel(3.0f * bx::kPi / 4.0f, 95.0f, 450.0f, 60.0f);
        m_vd.drawWheel(bx::kPi / 2.0f, 1150.0f, 450.0f, 30.0f);
        m_vd.drawWheel(bx::kPi / 4.0f, 1250.0f, 450.0f, 10.0f);

        // draw moving shape
        static float counter = 0.0f;
        counter += 0.01f;

        const float posX = m_width  / 2.0f + bx::sin(counter * 3.18378f) * (m_width / 2.0f);
        const float posY = m_height / 2.0f + bx::cos(counter) * (m_height / 2.0f);
        m_vd.drawCircle(posX, posY, 5.0f, 10.0f);

        m_vd.endFrame();

        // Advance to next frame. Rendering thread will be kicked to
        // process submitted rendering primitives.
        bgfx::frame();
        return 0;
    }

}