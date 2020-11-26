//
// Created by Administrator on 2020/11/24 0024.
//

#ifndef BGFX_STUDY_BGFX_APP_H
#define BGFX_STUDY_BGFX_APP_H

#include <atomic>
#include <stdatomic.h>
#include <mutex>

#include <bx/thread.h>

#define TYPE_NONE 0
#define TYPE_APP_INIT    1         /* init app */
#define TYPE_APP_PAUSE   2         /* pause app  not manul. eg: android's onPause */
#define TYPE_APP_PAUSE_MANULLY   3 /* pause app manul */
#define TYPE_APP_RENDER  4
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
    class BgfxApp;
    class Input;

    static Input* _input = NULL;

    class BgfxApp{
    public:
        /** there are two work mode:  render self and render trigger by events. */
        bool renderSelf = false;
        /**
         * init success means will draw after this.
         * @param pConfig  the config
         * @return  true if init success.
         */
        void doInit(bgfx::Init* init, entry::InitConfig* config);

        /**
         * called on pre init
         */
        virtual void onPreInit() = 0;
        /**
         * called on init
         */
        virtual void onInit() = 0;
        /**
         * called on draw.
         */
        virtual void onDraw() = 0;

        /**
         * called on destroy
         * @param lightly. lightly if it light , it means just call onDestroy and never change state.
         */
        virtual void onDestroy(bool lightly) = 0;

        /** real destroy. not change state
         * @param lightly if it light , it means just call onDestroy and never change state. also not clean lua ref.
         * */
        virtual void actDestroy(bool lightly = false);

        inline bool isDestroyed() {
            return getState() == APP_STATE_DESTROYED;
        }
        inline void quit() {
            //LOGD("LuaApp quit:...");
            setState(APP_STATE_TO_QUIT);
        }
        inline bool isRunning() {
            return getState() == APP_STATE_RUNNING;
        }
        inline void markRunning() {
            setState(APP_STATE_RUNNING);

        }
        inline void pause(){
            setState(APP_STATE_PAUSED);
        }
        inline bool isPaused(){
            return getState()== APP_STATE_PAUSED;
        }
        uint8_t getState();
        void setState(uint8_t s);

        /**
         * called on lifecycle 'onPause'. this is called outside. eg: android activity's onPause
         */
        virtual void onPause();
        /**
         * called on lifecycle 'onResume'. this is called outside.eg: android activity's onResume
         */
        virtual void onResume();

    private:
        std::atomic<uint8_t> state{APP_STATE_NONE};
    };

    typedef void (*EndTask)(AppController * holder);
    class AppController{
    public:
        BgfxApp* app;
        bgfx::Init* bgfx_init;
        entry::InitConfig* config;

        ~AppController();
        AppController();

        void quitApp();
        void destroyApp();

        void startLoop(entry::InitConfig *pConfig);
        void quitAll(EndTask task = NULL);
        void start(BgfxApp * app);

        void pause(bool manul = false);
        void resume();

        bool isRunning();
        void requestRender();

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
        EndTask task;

        CmdData(uint8_t type, void * data);
        CmdData(uint8_t type, EndTask task);
    };
}

#endif //BGFX_STUDY_BGFX_APP_H
