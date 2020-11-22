//
// Created by Administrator on 2020/11/22 0022.
//

#ifndef BGFX_STUDY_TIMEUTILS_HPP
#define BGFX_STUDY_TIMEUTILS_HPP

#include <stdint.h>
#include <bx/timer.h>

namespace h7{
    inline uint64_t nanoTime(){
        return bx::getHPCounter();
    }
}

#endif //BGFX_STUDY_TIMEUTILS_HPP
