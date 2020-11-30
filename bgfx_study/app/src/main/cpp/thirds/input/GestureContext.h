//
// Created by Administrator on 2020/11/22 0022.
//

#ifndef BGFX_STUDY_GESTURECONTEXT_H
#define BGFX_STUDY_GESTURECONTEXT_H

namespace h7{
    class Input;

    unsigned long long getCurrentEventTime();

    Input* getInput();

    void requestRender(long long luaPtr);

    void requestRender();

    void getDisplayInfo(long long luaPtr, int* out);
}

#endif //BGFX_STUDY_GESTURECONTEXT_H
