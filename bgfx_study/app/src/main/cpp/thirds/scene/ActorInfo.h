//
// Created by Administrator on 2020/12/23 0023.
//

#ifndef BGFX_STUDY_ACTORINFO_H
#define BGFX_STUDY_ACTORINFO_H

#include "../nancanvas/SkMatrix.h"

namespace h7{

    typedef struct WH{
        float w = 0;
        float h = 0;
    }WH;

    // should never use: new
    typedef struct ActorInfo{
        float x, y;
        float width, height;      // width, height

        float alpha = 1;          // alpha
        float scaleX = 1, scaleY = 1;

        float rotation = 0;   //in degree
        float transX = 0, transY = 0;

        void reset(){
            x = y = width = height = 0;
            alpha = scaleX = scaleY = 1;
            rotation = transX = transY = 0;
        }
    }ActorInfo;

    typedef struct ScrollInfo{
        static constexpr unsigned char SCROLL_DIRECTION_NONE  = 0;
        static constexpr unsigned char SCROLL_DIRECTION_UP    = 1;
        static constexpr unsigned char SCROLL_DIRECTION_DOWN  = 2;
        static constexpr unsigned char SCROLL_DIRECTION_LEFT  = 3;
        static constexpr unsigned char SCROLL_DIRECTION_RIGHT = 4;

        static constexpr unsigned char SCROLL_MODE_NONE = 0;
        static constexpr unsigned char SCROLL_MODE_SCROLLING     = 1;
        static constexpr unsigned char SCROLL_MODE_FLING     = 2;

        static constexpr unsigned char SCROLL_STATE_NONE = 0; //indicate is not scroll state changed
        static constexpr unsigned char SCROLL_STATE_IDLE = 1;
        static constexpr unsigned char SCROLL_STATE_DRAGGING = 2;
        static constexpr unsigned char SCROLL_STATE_SETTLING = 3;

        int scrollState = SCROLL_STATE_IDLE;
        float scrollX = 0;
        float scrollY = 0;
        unsigned char dir = SCROLL_DIRECTION_NONE;
        unsigned char mode = SCROLL_MODE_NONE;
    }ScrollInfo;


}

#endif //BGFX_STUDY_ACTORINFO_H
