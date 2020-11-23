//
// Created by Administrator on 2020/11/22 0022.
//

#ifndef BGFX_STUDY_TASK_H
#define BGFX_STUDY_TASK_H

#include <functional>
#include "system_wrappers/interface/timer_wrapper.h"

namespace h7{
    class SchedulerTask;

    class TaskContext{
    public:
        SchedulerTask* task; //null means invalid.
        void* userdata;
        bool once;
    };

    class SchedulerTask{
    private:
        std::function<void*(SchedulerTask*, void*)> func;
        int32_t _tid = -1;
        gn::TimerWrapper* _timer;
        bool _needReleaseTimer;

        TaskContext* _ctx;
        void* _result;
    public:
        SchedulerTask(gn::TimerWrapper* timer);
        SchedulerTask();
        ~SchedulerTask(){};

        void set(std::function<void*(SchedulerTask*,void*)> func);

        void cancel();
        bool isScheduled();

        bool schedule(uint32_t msec, void* userdata);
        bool schedulePeriod(uint32_t msec, void* userdata);

        inline bool schedule(float timeSeconds, void* userdata){
            return schedulePeriod((uint32_t)(1000 * timeSeconds), userdata);
        }
        inline bool schedulePeriod(float timeSeconds, void* userdata){
            return schedulePeriod((uint32_t)(1000 * timeSeconds), userdata);
        }
        inline void* getResult() {
            return _result;
        }
        //helper method for callback
        inline void* perform(void *ud) {
            return func ? func(this, ud) : NULL;
        }
        inline void setResult(void *result) {
            _result = result;
        }
    };

}

#endif //BGFX_STUDY_TASK_H
