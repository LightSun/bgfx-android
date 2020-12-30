//
// Created by Administrator on 2020/11/24 0024.
//

#ifndef BGFX_STUDY_BGFX_APP_H
#define BGFX_STUDY_BGFX_APP_H

#include <mutex>

#include <bx/thread.h>
#include "SkRefCnt.h"

#define TYPE_NONE 0
#define TYPE_APP_INIT    1         /* init app */
#define TYPE_APP_PAUSE   2         /* pause app  not manul. eg: android's onPause */
#define TYPE_APP_PAUSE_MANULLY   3 /* pause app manul */
#define TYPE_APP_RENDER  4
#define TYPE_APP_LAYOUT  5
#define TYPE_QUIT_ALL    10

#define APP_STATE_NONE    1
#define APP_STATE_RUNNING 2
#define APP_STATE_PAUSED  3
#define APP_STATE_TO_QUIT 4
#define APP_STATE_DESTROYED 5

namespace entry{
    struct InitConfig;
}
namespace bgfx{
    struct Init;
}

namespace h7{
    class AppController;
    class Application;
    class Input;

    typedef void (*AppTask)(AppController * holder);
    class AppController{
    public:
        Application* app;
        bgfx::Init* bgfx_init;
        entry::InitConfig* config;

        ~AppController();
        AppController();

        void quitApp();
        void destroyApp();

        void startLoop(entry::InitConfig *pConfig);
        void quitAll(AppTask task = NULL);
        void start(Application * app);

        void pause(bool manul = false);
        void resume();

        bool isRunning();
        void requestRender();
        void requestLayout();

    private:
        static int32_t threadFunc(bx::Thread* _thread, void* _userData);
        //bx thread can't be static
        bx::Thread m_thread;
        //can't use static
        std::mutex _mutex;
        std::condition_variable _condition;
    };
    class CmdData{
    public:
        void * data;
        uint8_t type = TYPE_NONE;
        AppTask task; // we can add a task to the thread.

        CmdData(uint8_t type, void * data);
        CmdData(uint8_t type, AppTask task);
    };
}

#endif //BGFX_STUDY_BGFX_APP_H
