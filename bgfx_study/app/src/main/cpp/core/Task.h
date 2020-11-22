//
// Created by Administrator on 2020/11/22 0022.
//

#ifndef BGFX_STUDY_TASK_H
#define BGFX_STUDY_TASK_H

#include <functional>

namespace h7{

    class Task{
    private:
        std::function<void*()> func;
    public:
        void set(std::function<void*()> func);
        void cancel();
        bool isScheduled();
        void schedule(float timeSeconds);
    };

}

#endif //BGFX_STUDY_TASK_H
