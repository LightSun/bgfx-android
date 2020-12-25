//
// Created by Administrator on 2020/11/30 0030.
//

#ifndef BGFX_STUDY_ALIGN_H
#define BGFX_STUDY_ALIGN_H

#include "../../core/stringbuilder.h"
#include "nancanvas/SkRect.h"

namespace h7 {
    namespace Align {
        static constexpr int center = 1 << 0;
        static constexpr int top = 1 << 1;
        static constexpr int bottom = 1 << 2;
        static constexpr int left = 1 << 3;
        static constexpr int right = 1 << 4;

        static constexpr int topLeft = top | left;
        static constexpr int topRight = top | right;
        static constexpr int bottomLeft = bottom | left;
        static constexpr int bottomRight = bottom | right;

        constexpr bool isLeft(int align) {
            return (align & left) != 0;
        }
        constexpr bool isRight(int align) {
            return (align & right) != 0;
        }
        constexpr bool isTop(int align) {
            return (align & top) != 0;
        }
        constexpr bool isBottom(int align) {
            return (align & bottom) != 0;
        }
        constexpr bool isCenterVertical(int align) {
            return (align & top) == 0 && (align & bottom) == 0;
        }
        constexpr bool isCenterHorizontal(int align) {
            return (align & left) == 0 && (align & right) == 0;
        }
        inline void applyAlign(SkRect& range, float w, float h, int aligns, SkRect& out){
            if((aligns & left) != 0){
                out.fLeft = range.fLeft;
                out.fRight = out.fLeft + w;
            } else if((aligns & right) != 0){
                out.fRight = range.fRight;
                out.fLeft = out.fRight - w;
            } else if((aligns & center) != 0){
                out.fLeft = range.fLeft + (range.width() - w) / 2;
                out.fRight = out.fLeft + w;
            }
            if((aligns & top) != 0){
                out.fTop = range.fTop;
                out.fBottom = out.fTop + h;
            }else if((aligns & bottom) != 0){
                out.fBottom = range.fBottom;
                out.fTop = out.fBottom - h;
            }else if((aligns & center) != 0){
                out.fTop = range.fTop + (range.height() - h) / 2;
                out.fBottom = out.fTop + h;
            }
        }
        inline void applyAlign(SkRect& range, float w, float h, int aligns, float& outLeft, float& outTop) {
            if ((aligns & left) != 0) {
                outLeft = range.fLeft;
            } else if ((aligns & right) != 0) {
                outLeft = range.fRight - w;
            } else if ((aligns & center) != 0) {
                outLeft = range.fLeft + (range.width() - w) / 2;
            }
            if((aligns & top) != 0){
                outTop = range.fTop;
            }else if((aligns & bottom) != 0){
                outTop = range.fBottom - h;
            }else if((aligns & center) != 0){
                outTop = range.fTop + (range.height() - h) / 2;
            }
        }
        inline void toString(int align, SB::StringBuilder& sb){
            if ((align & top) != 0)
                sb.append("top,");
            else if ((align & bottom) != 0)
                sb.append("bottom,");
            else
                sb.append("center,");
            if ((align & left) != 0)
                sb.append("left");
            else if ((align & right) != 0)
                sb.append("right");
            else
                sb.append("center");
        }
    }
}

#endif //BGFX_STUDY_ALIGN_H
