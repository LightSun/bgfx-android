//
// Created by Administrator on 2020/11/23 0023.
//

#include "Task.h"

namespace h7{

    SchedulerTask::SchedulerTask(gn::TimerWrapper *timer): _timer(timer), _needReleaseTimer(false),
        _ctx(NULL),_result(NULL) {}

    SchedulerTask::SchedulerTask(): _needReleaseTimer(true), _ctx(NULL), _result(NULL){
        _timer = gn::TimerWrapper::CreateTimer();
    }
    SchedulerTask::~SchedulerTask() {
        if(_needReleaseTimer && _timer != NULL){
            delete _timer;
            _timer = NULL;
        }
        if(_ctx){
            delete _ctx;
            _ctx = NULL;
        }
    }
    void SchedulerTask::set(std::function<void *(SchedulerTask* task,void *userData)> &func) {
        this->func = func;
    }
    void SchedulerTask::cancel() {
        if(_tid != -1){
            _timer->KillTimer(_tid);
            _tid = -1;
        }
        _ctx->task = NULL;
    }
    bool SchedulerTask::isScheduled() {
        return _tid != -1;
    }
    static void callback0(int32_t id, void* ud){
        TaskContext* ctx = static_cast<TaskContext *>(ud);
        auto pTask = ctx->task;
        if(pTask != NULL){
            auto result = pTask->perform(ctx->userdata);
            pTask->setResult(result);
            if(ctx->once){
                pTask->cancel();
            }
        }
    }
    bool SchedulerTask::schedule(uint32_t msec, void* userdata) {
        if(isScheduled()){
            return false;
        }
        _ctx = new TaskContext();
        _ctx->task = this;
        _ctx->userdata = userdata;
        _ctx->once = true;
        _tid = _timer->SetTimer(msec, callback0, _ctx);
        return true;
    }
    bool SchedulerTask::schedulePeriod(uint32_t msec, void *userdata) {
        if(isScheduled()){
            return false;
        }
        _ctx = new TaskContext();
        _ctx->task = this;
        _ctx->userdata = userdata;
        _ctx->once = false;
        _tid = _timer->SetTimer(msec, callback0, _ctx);
        return true;
    }
}