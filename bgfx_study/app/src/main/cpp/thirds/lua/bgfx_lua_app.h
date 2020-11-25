//
// Created by Administrator on 2020/8/22 0022.
//

#ifndef BGFX_STUDY_BGFX_LUA_APP_H
#define BGFX_STUDY_BGFX_LUA_APP_H

#include <mutex>

#include "bgfx_app.h"
#include <bx/thread.h>


//防止指令重排. linux 内核可用 cpu_relax函数（效果相同）
//#define barrier() __asm__ __volatile__("": : :"memory")

/*#define lua_runMain(L) \
    LuaAppHolder *pHolder = Bgfx_lua_app::getAppHolder(L); \
    pHolder->config->RunMain(reinterpret_cast<long>(L));*/
namespace h7{
    class LuaAppHolder;
}
struct lua_State;

namespace Bgfx_lua_app{

    bool startApp(long ptr, entry::InitConfig *pConfig);
    bgfx::Init* requireInit(lua_State* L);
    h7::LuaAppHolder* getAppHolder(lua_State* L);

    /**
     * release platform window.
     */
    void releaseWindow(long ptr);

    void onLifecycle(long luaPtr, int mark);
}

namespace h7{
    class LuaApp;
    class LuaAppHolder;

    typedef void (*EndTask)(LuaAppHolder * holder);
    typedef const char* FUNC_NAME;

    class LuaAppHolder{
    public:
        BgfxApp* app;
        bgfx::Init* bgfx_init;
        entry::InitConfig* config;

        ~LuaAppHolder();
        LuaAppHolder();

        void quitApp();
        void destroyApp();

        void startLoop(entry::InitConfig *pConfig);
        void quitAll(EndTask task = NULL);
        void start(BgfxApp * app);

        void pause();
        void resume();

        bool isRunning();

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

    class LuaApp: public h7::BgfxApp{

    public:
        /**
          * create lua app with init, draw, destroy function.
          * @param L the lua state
          * @param preInit the pre init function name ,must be global stack
          * @param func_init the init function name ,must be global stack
          * @param func_draw the draw function name ,must be global stack
          * @param func_destroy the draw destroy name ,must be global stack
          */
        LuaApp(lua_State* L, FUNC_NAME preInit, FUNC_NAME func_init, FUNC_NAME func_draw, FUNC_NAME func_destroy);

        //called in sub-thread.
        void onDraw();

        void onPreInit();
        void onInit();
        void onDestroy(bool lightly);

        void onPause();

        void onResume();

    private:
        inline void _callLuaDestroy();
        lua_State* L;
        FUNC_NAME func_preInit;
        FUNC_NAME func_init;
        FUNC_NAME func_draw;
        FUNC_NAME func_destroy;
    };
}

#endif //BGFX_STUDY_BGFX_LUA_APP_H
