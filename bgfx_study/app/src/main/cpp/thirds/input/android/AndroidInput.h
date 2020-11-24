//
// Created by Administrator on 2020/11/24 0024.
//

#ifndef BGFX_STUDY_ANDROIDINPUT_H
#define BGFX_STUDY_ANDROIDINPUT_H

#include "../Input.h"
#include "../../utils/Pool.hpp"

namespace h7 {
    class OnKeyListener{

    };

    //TODO latter impl Input
    class AndroidInput : public Input {

    public:
        const static int NUM_TOUCHES = 20;
        const static int SUPPORTED_KEYS = 260;
        int* touchX;
        int* touchY;
        int* deltaX;
        int* deltaY;
        bool* touched;

        int* button;
        int* realId;
        float* pressure;

        bool hasMultitouch;
        int keyCount = 0;

        int len_realId = NUM_TOUCHES;

        bool keyJustPressed = false;
        bool keys[SUPPORTED_KEYS];
        bool justPressedKeys[SUPPORTED_KEYS];
        bool justPressedButtons[NUM_TOUCHES];

        Pool<KeyEvent*> usedKeyEvents = Pool<KeyEvent*>(16, 100);
        Pool<TouchEvent*> usedTouchEvents = Pool<TouchEvent*>(16, 100);

        Array<OnKeyListener*> keyListeners;
        Array<KeyEvent*> keyEvents;
        Array<TouchEvent*> touchEvents;

        AndroidInput();
        ~AndroidInput();

        int getFreePointerIndex();
        int lookUpPointerIndex(int pointerId);

    private:
        void release();
        void ensureSize(int expect);
    };


}

#endif //BGFX_STUDY_ANDROIDINPUT_H
