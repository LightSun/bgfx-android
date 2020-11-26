//
// Created by Administrator on 2020/11/26 0026.
//

#include <sys/time.h>
#include "../GestureContext.h"

namespace h7{

    unsigned long long getCurrentEventTime(){
        struct timeval xTime;
        gettimeofday(&xTime, NULL);
        //nsec
        unsigned long long now = (unsigned long long)((xTime.tv_sec * 1000000000) + (xTime.tv_usec * 1000));
        return now;
    }
}