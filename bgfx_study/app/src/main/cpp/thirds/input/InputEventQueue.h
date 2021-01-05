//
// Created by Administrator on 2020/11/27 0027.
//

#ifndef BGFX_STUDY_INPUTEVENTQUEUE_H
#define BGFX_STUDY_INPUTEVENTQUEUE_H

#include <mutex>
#include "InputProcessor.h"
#include "utils/Array.h"
#include "lua/SkRefCnt.h"

namespace h7 {
    class InputEventQueue : public InputProcessor {
    public:
        static constexpr int SKIP = -1;
        static constexpr int KEY_DOWN = 0;
        static constexpr int KEY_UP = 1;
        static constexpr int KEY_TYPED = 2;
        static constexpr int TOUCH_DOWN = 3;
        static constexpr int TOUCH_UP = 4;
        static constexpr int TOUCH_DRAGGED = 5;
        static constexpr int MOUSE_MOVED = 6;
        static constexpr int SCROLLED = 7;

        InputProcessor *processor;
        IntArray queue;
        IntArray processingQueue;
        long long currentEventTime;

        inline void setProcessor(InputProcessor* processor) {
            this->processor = processor;
        }
        inline InputProcessor* getProcessor() {
            return processor;
        }

        void drain ();

        bool keyDown (int keycode);
        bool keyUp (int keycode);
        bool keyTyped (char ch);
        bool touchDown (int screenX, int screenY, int pointer, int button);
        bool touchUp (int screenX, int screenY, int pointer, int button);
        bool touchDragged (int screenX, int screenY, int pointer);
        bool mouseMoved (int screenX, int screenY);
        bool scrolled(float amountX, float amountY);
        inline long long getCurrentEventTime(){
            return currentEventTime;
        }
        inline void lock() {
            _mutex.lock();
        }
        inline void unlock() {
            _mutex.unlock();
        }
    private:
        std::mutex _mutex;
        inline int next (int nextType, int i);
        inline void queueTime ();
    };
}

#endif //BGFX_STUDY_INPUTEVENTQUEUE_H
