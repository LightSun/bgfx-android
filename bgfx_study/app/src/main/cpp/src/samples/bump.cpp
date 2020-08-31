//
// Created by Administrator on 2020/8/31 0031.
//

#include <bgfx_utils.h>
#include <bx/timer.h>
#include "include/bump.h"


namespace heaven7_Bgfx_demo {
    struct PosNormalTangentTexcoordVertex
    {
        float m_x;
        float m_y;
        float m_z;
        uint32_t m_normal;
        uint32_t m_tangent;
        int16_t m_u;
        int16_t m_v;

        static void init()
        {
            ms_layout
                    .begin()
                    .add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
                    .add(bgfx::Attrib::Normal,    4, bgfx::AttribType::Uint8, true, true)
                    .add(bgfx::Attrib::Tangent,   4, bgfx::AttribType::Uint8, true, true)
                    .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Int16, true, true)
                    .end();
        }

        static bgfx::VertexLayout ms_layout;
    };

    bgfx::VertexLayout PosNormalTangentTexcoordVertex::ms_layout;

    static PosNormalTangentTexcoordVertex s_cubeVertices[24] =
            {
                    {-1.0f,  1.0f,  1.0f, encodeNormalRgba8( 0.0f,  0.0f,  1.0f), 0,      0,      0 },
                    { 1.0f,  1.0f,  1.0f, encodeNormalRgba8( 0.0f,  0.0f,  1.0f), 0, 0x7fff,      0 },
                    {-1.0f, -1.0f,  1.0f, encodeNormalRgba8( 0.0f,  0.0f,  1.0f), 0,      0, 0x7fff },
                    { 1.0f, -1.0f,  1.0f, encodeNormalRgba8( 0.0f,  0.0f,  1.0f), 0, 0x7fff, 0x7fff },
                    {-1.0f,  1.0f, -1.0f, encodeNormalRgba8( 0.0f,  0.0f, -1.0f), 0,      0,      0 },
                    { 1.0f,  1.0f, -1.0f, encodeNormalRgba8( 0.0f,  0.0f, -1.0f), 0, 0x7fff,      0 },
                    {-1.0f, -1.0f, -1.0f, encodeNormalRgba8( 0.0f,  0.0f, -1.0f), 0,      0, 0x7fff },
                    { 1.0f, -1.0f, -1.0f, encodeNormalRgba8( 0.0f,  0.0f, -1.0f), 0, 0x7fff, 0x7fff },
                    {-1.0f,  1.0f,  1.0f, encodeNormalRgba8( 0.0f,  1.0f,  0.0f), 0,      0,      0 },
                    { 1.0f,  1.0f,  1.0f, encodeNormalRgba8( 0.0f,  1.0f,  0.0f), 0, 0x7fff,      0 },
                    {-1.0f,  1.0f, -1.0f, encodeNormalRgba8( 0.0f,  1.0f,  0.0f), 0,      0, 0x7fff },
                    { 1.0f,  1.0f, -1.0f, encodeNormalRgba8( 0.0f,  1.0f,  0.0f), 0, 0x7fff, 0x7fff },
                    {-1.0f, -1.0f,  1.0f, encodeNormalRgba8( 0.0f, -1.0f,  0.0f), 0,      0,      0 },
                    { 1.0f, -1.0f,  1.0f, encodeNormalRgba8( 0.0f, -1.0f,  0.0f), 0, 0x7fff,      0 },
                    {-1.0f, -1.0f, -1.0f, encodeNormalRgba8( 0.0f, -1.0f,  0.0f), 0,      0, 0x7fff },
                    { 1.0f, -1.0f, -1.0f, encodeNormalRgba8( 0.0f, -1.0f,  0.0f), 0, 0x7fff, 0x7fff },
                    { 1.0f, -1.0f,  1.0f, encodeNormalRgba8( 1.0f,  0.0f,  0.0f), 0,      0,      0 },
                    { 1.0f,  1.0f,  1.0f, encodeNormalRgba8( 1.0f,  0.0f,  0.0f), 0, 0x7fff,      0 },
                    { 1.0f, -1.0f, -1.0f, encodeNormalRgba8( 1.0f,  0.0f,  0.0f), 0,      0, 0x7fff },
                    { 1.0f,  1.0f, -1.0f, encodeNormalRgba8( 1.0f,  0.0f,  0.0f), 0, 0x7fff, 0x7fff },
                    {-1.0f, -1.0f,  1.0f, encodeNormalRgba8(-1.0f,  0.0f,  0.0f), 0,      0,      0 },
                    {-1.0f,  1.0f,  1.0f, encodeNormalRgba8(-1.0f,  0.0f,  0.0f), 0, 0x7fff,      0 },
                    {-1.0f, -1.0f, -1.0f, encodeNormalRgba8(-1.0f,  0.0f,  0.0f), 0,      0, 0x7fff },
                    {-1.0f,  1.0f, -1.0f, encodeNormalRgba8(-1.0f,  0.0f,  0.0f), 0, 0x7fff, 0x7fff },
            };

    static const uint16_t s_cubeIndices[36] =
            {
                    0,  2,  1,
                    1,  2,  3,
                    4,  5,  6,
                    5,  7,  6,

                    8, 10,  9,
                    9, 10, 11,
                    12, 13, 14,
                    13, 15, 14,

                    16, 18, 17,
                    17, 18, 19,
                    20, 21, 22,
                    21, 23, 22,
            };



    void BumpDemo::init() {
        m_debug  = BGFX_DEBUG_NONE;
        m_reset  = BGFX_RESET_VSYNC;
        BaseDemo::init();

        // Get renderer capabilities info.
        const bgfx::Caps* caps = bgfx::getCaps();
        m_instancingSupported = 0 != (caps->supported & BGFX_CAPS_INSTANCING);

        // Create vertex stream declaration.
        PosNormalTangentTexcoordVertex::init();

        calcTangents(s_cubeVertices
                , BX_COUNTOF(s_cubeVertices)
                , PosNormalTangentTexcoordVertex::ms_layout
                , s_cubeIndices
                , BX_COUNTOF(s_cubeIndices)
        );

        // Create static vertex buffer.
        m_vbh = bgfx::createVertexBuffer(
                bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices) )
                , PosNormalTangentTexcoordVertex::ms_layout
        );

        // Create static index buffer.
        m_ibh = bgfx::createIndexBuffer(bgfx::makeRef(s_cubeIndices, sizeof(s_cubeIndices) ) );

        // Create texture sampler uniforms.
        s_texColor  = bgfx::createUniform("s_texColor",  bgfx::UniformType::Sampler);
        s_texNormal = bgfx::createUniform("s_texNormal", bgfx::UniformType::Sampler);

        m_numLights = 4;
        u_lightPosRadius = bgfx::createUniform("u_lightPosRadius", bgfx::UniformType::Vec4, m_numLights);
        u_lightRgbInnerR = bgfx::createUniform("u_lightRgbInnerR", bgfx::UniformType::Vec4, m_numLights);

        // Create program from shaders.
        m_program = loadProgram(m_instancingSupported ? "vs_bump_instanced" : "vs_bump", "fs_bump");

        // Load diffuse texture.
        m_textureColor = loadTexture("runtime/textures/fieldstone-rgba.dds");

        // Load normal texture.
        m_textureNormal = loadTexture("runtime/textures/fieldstone-n.dds");

        m_timeOffset = bx::getHPCounter();

    }

   void BumpDemo::destroy() {
       // Cleanup.
       bgfx::destroy(m_ibh);
       bgfx::destroy(m_vbh);
       bgfx::destroy(m_program);
       bgfx::destroy(m_textureColor);
       bgfx::destroy(m_textureNormal);
       bgfx::destroy(s_texColor);
       bgfx::destroy(s_texNormal);
       bgfx::destroy(u_lightPosRadius);
       bgfx::destroy(u_lightRgbInnerR);

       BaseDemo::destroy();
   }

   int BumpDemo::draw() {
       int m_width = config.win_width;
       int m_height = config.win_height;
       // Set view 0 default viewport.
       bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height) );

       // This dummy draw call is here to make sure that view 0 is cleared
       // if no other draw calls are submitted to view 0.
       bgfx::touch(0);

       float time = (float)( (bx::getHPCounter()-m_timeOffset)/double(bx::getHPFrequency() ));

       const bx::Vec3 at  = { 0.0f, 0.0f,  0.0f };
       const bx::Vec3 eye = { 0.0f, 0.0f, -7.0f };

       // Set view and projection matrix for view 0.
       {
           float view[16];
           bx::mtxLookAt(view, eye, at);

           float proj[16];
           bx::mtxProj(proj, 60.0f, float(m_width) / float(m_height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
           bgfx::setViewTransform(0, view, proj);

           // Set view 0 default viewport.
           bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height) );
       }

       float lightPosRadius[4][4];
       for (uint32_t ii = 0; ii < m_numLights; ++ii)
       {
           lightPosRadius[ii][0] = bx::sin( (time*(0.1f + ii*0.17f) + ii*bx::kPiHalf*1.37f ) )*3.0f;
           lightPosRadius[ii][1] = bx::cos( (time*(0.2f + ii*0.29f) + ii*bx::kPiHalf*1.49f ) )*3.0f;
           lightPosRadius[ii][2] = -2.5f;
           lightPosRadius[ii][3] = 3.0f;
       }

       bgfx::setUniform(u_lightPosRadius, lightPosRadius, m_numLights);

       float lightRgbInnerR[4][4] =
               {
                       { 1.0f, 0.7f, 0.2f, 0.8f },
                       { 0.7f, 0.2f, 1.0f, 0.8f },
                       { 0.2f, 1.0f, 0.7f, 0.8f },
                       { 1.0f, 0.4f, 0.2f, 0.8f },
               };

       bgfx::setUniform(u_lightRgbInnerR, lightRgbInnerR, m_numLights);

       const uint16_t instanceStride = 64;
       const uint16_t numInstances   = 3;

       if (m_instancingSupported)
       {
           // Write instance data for 3x3 cubes.
           for (uint32_t yy = 0; yy < 3; ++yy)
           {
               if (numInstances == bgfx::getAvailInstanceDataBuffer(numInstances, instanceStride) )
               {
                   bgfx::InstanceDataBuffer idb;
                   bgfx::allocInstanceDataBuffer(&idb, numInstances, instanceStride);

                   uint8_t* data = idb.data;

                   for (uint32_t xx = 0; xx < 3; ++xx)
                   {
                       float* mtx = (float*)data;
                       bx::mtxRotateXY(mtx, time*0.023f + xx*0.21f, time*0.03f + yy*0.37f);
                       mtx[12] = -3.0f + float(xx)*3.0f;
                       mtx[13] = -3.0f + float(yy)*3.0f;
                       mtx[14] = 0.0f;

                       data += instanceStride;
                   }

                   // Set instance data buffer.
                   bgfx::setInstanceDataBuffer(&idb, 0, numInstances);

                   // Set vertex and index buffer.
                   bgfx::setVertexBuffer(0, m_vbh);
                   bgfx::setIndexBuffer(m_ibh);

                   // Bind textures.
                   bgfx::setTexture(0, s_texColor,  m_textureColor);
                   bgfx::setTexture(1, s_texNormal, m_textureNormal);

                   // Set render states.
                   bgfx::setState(0
                                  | BGFX_STATE_WRITE_RGB
                                  | BGFX_STATE_WRITE_A
                                  | BGFX_STATE_WRITE_Z
                                  | BGFX_STATE_DEPTH_TEST_LESS
                                  | BGFX_STATE_MSAA
                   );

                   // Submit primitive for rendering to view 0.
                   bgfx::submit(0, m_program);
               }
           }
       }
       else
       {
           for (uint32_t yy = 0; yy < 3; ++yy)
           {
               for (uint32_t xx = 0; xx < 3; ++xx)
               {
                   float mtx[16];
                   bx::mtxRotateXY(mtx, time*0.023f + xx*0.21f, time*0.03f + yy*0.37f);
                   mtx[12] = -3.0f + float(xx)*3.0f;
                   mtx[13] = -3.0f + float(yy)*3.0f;
                   mtx[14] = 0.0f;

                   // Set transform for draw call.
                   bgfx::setTransform(mtx);

                   // Set vertex and index buffer.
                   bgfx::setVertexBuffer(0, m_vbh);
                   bgfx::setIndexBuffer(m_ibh);

                   // Bind textures.
                   bgfx::setTexture(0, s_texColor,  m_textureColor);
                   bgfx::setTexture(1, s_texNormal, m_textureNormal);

                   // Set render states.
                   bgfx::setState(0
                                  | BGFX_STATE_WRITE_RGB
                                  | BGFX_STATE_WRITE_A
                                  | BGFX_STATE_WRITE_Z
                                  | BGFX_STATE_DEPTH_TEST_LESS
                                  | BGFX_STATE_MSAA
                   );

                   // Submit primitive for rendering to view 0.
                   bgfx::submit(0, m_program);
               }
           }
       }

       // Advance to next frame. Rendering thread will be kicked to
       // process submitted rendering primitives.
       bgfx::frame();
       return 0;
   }
}