//
// Created by Administrator on 2020/8/22 0022.
//

#ifndef BGFX_STUDY_BGFX_LUA_APP_H
#define BGFX_STUDY_BGFX_LUA_APP_H

#include "bgfx_app.h"

//防止指令重排. linux 内核可用 cpu_relax函数（效果相同）
//#define barrier() __asm__ __volatile__("": : :"memory")

/*#define lua_runMain(L) \
    LuaAppHolder *pHolder = Bgfx_lua_app::getAppHolder(L); \
    pHolder->config->RunMain(reinterpret_cast<long>(L));*/
namespace h7{
    class AppController;
}
struct lua_State;

namespace h7{

    bool startApp(long long ptr, entry::InitConfig *pConfig);
    bgfx::Init* requireInit(lua_State* L);
    h7::AppController* getAppHolder(lua_State* L);
    /**
     * release platform window.
     */
    void releaseWindow(long ptr);

    void onLifecycle(long luaPtr, int mark);
}

namespace h7{
    class LuaApp;
    class AppController;

    class LuaApp: public BgfxApp{

    public:
        /**
          * create lua app with init, draw, destroy function.
          * order: preinit, init, draw, destroy
          * @param L the lua state
          */
        LuaApp(lua_State* L);
        ~LuaApp();

        //called in sub-thread.
        void onDraw();

        void onPreInit();
        void onInit();
        void onDestroy(bool lightly);

        void onPause();

        void onResume();

    private:
        inline void _callLuaDestroy();
        inline void _release();
        lua_State* L;

        int ref_destroy;
        int ref_draw;
        int ref_init;
        int ref_preInit;
    };
}

#endif //BGFX_STUDY_BGFX_LUA_APP_H
