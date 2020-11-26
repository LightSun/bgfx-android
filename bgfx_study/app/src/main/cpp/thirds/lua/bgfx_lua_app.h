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

    bool startApp(long ptr, entry::InitConfig *pConfig);
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

    typedef const char* FUNC_NAME;
    class LuaApp: public BgfxApp{

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
