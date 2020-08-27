//
// Created by Administrator on 2020/8/10 0010.
//

#ifndef BGFX_STUDY_FONT_10_H
#define BGFX_STUDY_FONT_10_H

#include "../../../core/common.h"
#include "../../base_demo.h"
#include "bgfx_utils.h"

#include <bx/timer.h>
#include <bx/string.h>
#include <bx/math.h>

#include "font/font_manager.h"
#include "font/text_buffer_manager.h"
#include "font_10.h"
//#include "entry/input.h"

#include <iconfontheaders/icons_font_awesome.h>
#include <iconfontheaders/icons_kenney.h>

#include <wchar.h>

namespace heaven7_Bgfx_demo{

    static const char* s_fontFilePath[] =
            {
                    "font/droidsans.ttf",
                    "font/chp-fire.ttf",
                    "font/bleeding_cowboys.ttf",
                    "font/mias_scribblings.ttf",
                    "font/ruritania.ttf",
                    "font/signika-regular.ttf",
                    "font/five_minutes.otf",
            };


    class FontDemo: public BaseDemo{

    public:
        void init() override;
        int draw() override;
        void destroy() override;

    private:
       // entry::MouseState m_mouseState;

        FontManager* m_fontManager;
        TextBufferManager* m_textBufferManager;

        FontHandle m_visitor10;
        TrueTypeHandle m_fontAwesomeTtf;
        TrueTypeHandle m_fontKenneyTtf;
        FontHandle m_fontAwesome72;
        FontHandle m_fontKenney64;
        TrueTypeHandle m_visitorTtf;

        TextBufferHandle m_transientText;
        TextBufferHandle m_staticText;

        static const uint32_t numFonts = BX_COUNTOF(s_fontFilePath);

        TrueTypeHandle m_fontFiles[numFonts];
        FontHandle m_fonts[numFonts];
    };
}

#endif //BGFX_STUDY_FONT_10_H
