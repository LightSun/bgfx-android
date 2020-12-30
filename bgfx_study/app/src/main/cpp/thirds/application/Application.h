//
// Created by Administrator on 2020/12/7 0007.
//

#ifndef BGFX_STUDY_APPLICATION_H
#define BGFX_STUDY_APPLICATION_H

#include <atomic>
#include <stdatomic.h>

#include "Context.h"

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

typedef struct NVGcontext NVGcontext;

namespace h7{
    class Input;
    class Stage;
    class ApplicationListener;

    class Application{
    public:
        ~Application();

        Input* input = nullptr;
        Stage* stage = nullptr;
        ApplicationListener* listener = nullptr;

        static Application* get();

        void initStage(int x, int y, int w, int h);
        Context& getContext();

        //--------- etc.  ---------------
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

        void layout();

    private:
        NVGcontext* nvgCtx = nullptr;
        Context context;
        std::atomic<uint8_t> state{APP_STATE_NONE};
    };
}

#endif //BGFX_STUDY_APPLICATION_H
