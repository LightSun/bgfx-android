//
// Created by Administrator on 2020/12/7 0007.
//

#ifndef BGFX_STUDY_APPLICATION_H
#define BGFX_STUDY_APPLICATION_H

#include "ApplicationListener.h"

namespace h7{
    class Input;

    class Application{
    public:
        Input* input;
        ApplicationListener* listener;

        static Application* get();
    };

    static Application _app;

    Application* Application::get() {
        return &_app;
    }
}

#endif //BGFX_STUDY_APPLICATION_H
