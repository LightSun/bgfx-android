
//
// Created by Administrator on 2021/1/5 0005.
//

#ifndef BGFX_STUDY_LOCKDELEGATE_H
#define BGFX_STUDY_LOCKDELEGATE_H

#include <mutex>

namespace h7{

    class ILock{
    public:
        virtual void lock() {
        }
        virtual void unlock() {
        }
    };

    class SimpleLock: public ILock{
    public:
        inline void lock() {
            std::unique_lock<std::mutex> lock(_mutex);
        }
        inline void unlock() {
            std::unique_lock<std::mutex> unlock(_mutex);
        }
    private:
        std::mutex _mutex;
    };
}

#endif //BGFX_STUDY_LOCKDELEGATE_H
