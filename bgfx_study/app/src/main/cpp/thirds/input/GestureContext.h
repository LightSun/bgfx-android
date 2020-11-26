//
// Created by Administrator on 2020/11/22 0022.
//

#ifndef BGFX_STUDY_GESTURECONTEXT_H
#define BGFX_STUDY_GESTURECONTEXT_H

namespace h7{

    unsigned long long getCurrentEventTime();

    bool isTouched(int button);

    void requestRender();

    int getDisplayWidth();

    int getDisplayHeight();
}

#endif //BGFX_STUDY_GESTURECONTEXT_H
