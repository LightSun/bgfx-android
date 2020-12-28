//
// Created by Administrator on 2020/11/26 0026.
//

#include <sys/time.h>
#include "../GestureContext.h"

namespace h7{

    //in nesc
    unsigned long long getCurrentTime(){
        struct timeval xTime;
        gettimeofday(&xTime, nullptr);
        //nsec
        unsigned long long now =((xTime.tv_sec * 1000000000) + (xTime.tv_usec * 1000));
        return now;
    }
}