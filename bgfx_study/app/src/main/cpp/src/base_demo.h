//
// Created by Administrator on 2020/8/2 0002.
//

#ifndef BGFX_STUDY_BASE_DEMO_H
#define BGFX_STUDY_BASE_DEMO_H

#include "bgfx/bgfx.h"
#include <bx/bx.h>
#include <bx/thread.h>
#include "android_pri.h"
//extern "C" {

namespace heaven7_Bgfx_demo {

    typedef struct InitConfig {
        void *window;
        int win_width;
        int win_height;
    } InitConfig;

    class BaseDemo {
    public:
        BaseDemo();
        virtual void init(InitConfig* config);

        virtual int draw() = 0;

        virtual void destroy();

        bool isDestroyed();

        void startLoop();

    private:
        static int32_t threadFunc(bx::Thread* _thread, void* _userData);

    protected:
        bool destroyed;
        InitConfig config;
        int m_debug;
        int m_reset;
        bx::Thread m_thread;
    };
}
//}
#endif //BGFX_STUDY_BASE_DEMO_H
