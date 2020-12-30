//
// Created by Administrator on 2020/11/22 0022.
//

#ifndef BGFX_STUDY_GESTURECONTEXT_H
#define BGFX_STUDY_GESTURECONTEXT_H

namespace h7{
    class Input;

    unsigned long long getCurrentTime();
    unsigned long long getCurrentTimeInMsec();

    Input* getInput();
    void* setInput(Input* input);

    void requestRender();

    void getDisplayInfo(int* out);
}

#endif //BGFX_STUDY_GESTURECONTEXT_H
