//
// Created by Administrator on 2020/12/8 0008.
//

#include "Application.h"

namespace h7{
    static Application _app;

    Application* Application::get() {
        return &_app;
    }
}