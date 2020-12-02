//
// Created by Administrator on 2020/11/29 0029.
//

#ifndef BGFX_STUDY_VIEWPORT_H
#define BGFX_STUDY_VIEWPORT_H

#include "lua/SkRefCnt.h"

namespace h7 {
    class Viewport: public SkRefCnt{
    public:
        int x, y, width, height;

        inline bool isInside(int screenX, int screenY) {
            return screenX >= x && screenX < x + width && screenY >= y && screenY < y + height;
        }
    };
}

#endif //BGFX_STUDY_VIEWPORT_H
