//
// Created by Administrator on 2020/8/2 0002.
//

#include "include/hello_world.h"
#include "logo.h"

namespace heaven7_Bgfx_demo {

    void HelloWorldDemo::init(heaven7_Bgfx_demo::InitConfig *config) {
        m_reset = BGFX_RESET_VSYNC;
        m_debug = BGFX_DEBUG_TEXT;
        BaseDemo::init(config);
    }

    void HelloWorldDemo::draw() {
        // Set view 0 default viewport.
        bgfx::setViewRect(0, 0, 0, uint16_t(config.win_width), uint16_t(config.win_height) );

        // This dummy draw call is here to make sure that view 0 is cleared
        // if no other draw calls are submitted to view 0.
        bgfx::touch(0);

        // Use debug font to print information about this example.
        bgfx::dbgTextClear();
        bgfx::dbgTextImage(
                bx::max<uint16_t>(uint16_t(config.win_width /2/8 ), 20)-20
                , bx::max<uint16_t>(uint16_t(config.win_height/2/16),  6)-6
                , 40
                , 12
                , s_logo
                , 160
        );
        bgfx::dbgTextPrintf(0, 1, 0x0f, "Color can be changed with ANSI \x1b[9;me\x1b[10;ms\x1b[11;mc\x1b[12;ma\x1b[13;mp\x1b[14;me\x1b[0m code too.");

        bgfx::dbgTextPrintf(80, 1, 0x0f, "\x1b[;0m    \x1b[;1m    \x1b[; 2m    \x1b[; 3m    \x1b[; 4m    \x1b[; 5m    \x1b[; 6m    \x1b[; 7m    \x1b[0m");
        bgfx::dbgTextPrintf(80, 2, 0x0f, "\x1b[;8m    \x1b[;9m    \x1b[;10m    \x1b[;11m    \x1b[;12m    \x1b[;13m    \x1b[;14m    \x1b[;15m    \x1b[0m");

        const bgfx::Stats* stats = bgfx::getStats();
        bgfx::dbgTextPrintf(0, 2, 0x0f, "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters."
                , stats->width
                , stats->height
                , stats->textWidth
                , stats->textHeight
        );

        // Advance to next frame. Rendering thread will be kicked to
        // process submitted rendering primitives.
        bgfx::frame();
    }
}