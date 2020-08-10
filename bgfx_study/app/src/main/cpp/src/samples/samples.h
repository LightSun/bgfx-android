//
// Created by Administrator on 2020/8/2 0002.
//

#ifndef BGFX_STUDY_SAMPLES_H
#define BGFX_STUDY_SAMPLES_H

#include "../base_demo.h"
#include "include/hello_world.h"
#include "include/curbes.h"
#include "include/font_10.h"

namespace heaven7_Bgfx_demo{
    template <typename T> BaseDemo* create0();
#define DEF_METHOD_NAME(T)                           \
    template <> BaseDemo* create0<T>() {   \
        BaseDemo* demo = new T(); \
        return demo;          \
    }
    DEF_METHOD_NAME(HelloWorldDemo)
    DEF_METHOD_NAME(CurbesDemo)
    DEF_METHOD_NAME(FontDemo)

    template <typename T> BaseDemo* createDemo(InitConfig* config){
        BaseDemo* demo = create0<T>();
        demo->setInitConfig(config);
        return demo;
    }
}

#endif //BGFX_STUDY_SAMPLES_H
