//
// Created by Administrator on 2020/11/24 0024.
//

#ifndef BGFX_STUDY_ANDROIDINPUTJNI_H
#define BGFX_STUDY_ANDROIDINPUTJNI_H

#include "WeakObjectM.hpp"

#define LEN_MOTION_INFO 4

namespace h7{
    class KeyEventWrapper{

    };

    class MotionEventWrapper:public GlobalObjectM{
    public:
        static const int ACTION_DOWN = 0;
        static const int ACTION_UP = 1;
        static const int ACTION_MOVE = 2;
        static const int ACTION_CANCEL = 3;
        static const int ACTION_OUTSIDE = 4;
        static const int ACTION_POINTER_DOWN = 5;
        static const int ACTION_POINTER_UP = 6;
        static const int ACTION_HOVER_MOVE = 7;
        static const int ACTION_SCROLL = 8;

        int action;
        int pointerCount;
        int pointerIndex;
        int pointerId;
        long long timeStamp;
        int x, y;
        int buttonState;
        float pressure;

        float tmpArr[LEN_MOTION_INFO];

        MotionEventWrapper();

        inline int getPointerId(){ return (int)(tmpArr[0]);}
        inline float getX(){ return tmpArr[1];}
        inline float getY(){ return tmpArr[2];}
        inline float getPressure(){ return tmpArr[3];}
        /**
         * recycle java object
         */
        void jRecycle();
        void getEventInfo(int pointerIndex);
    };
}

#endif //BGFX_STUDY_ANDROIDINPUTJNI_H
