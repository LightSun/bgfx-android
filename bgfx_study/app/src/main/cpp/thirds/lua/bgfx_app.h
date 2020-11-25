//
// Created by Administrator on 2020/11/24 0024.
//

#ifndef BGFX_STUDY_BGFX_APP_H
#define BGFX_STUDY_BGFX_APP_H

#include <atomic>
#include <stdatomic.h>

#define TYPE_NONE 0
#define TYPE_LUA_APP_INIT    1
#define TYPE_LUA_APP_PAUSE   2
#define TYPE_QUIT_ALL        10

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

    class BgfxApp{

    public:

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
}

#endif //BGFX_STUDY_BGFX_APP_H
