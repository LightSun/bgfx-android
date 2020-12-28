//
// Created by Administrator on 2020/7/3 0003.
//

#ifndef FFMPEGOPENGLDEMO_VIDEOCONFIG_H
#define FFMPEGOPENGLDEMO_VIDEOCONFIG_H

#include "bx/debug.h"

#define SkASSERT(cond) static_cast<void>(0)
#define SkDebugf(fmt, ...)  bx::debugPrintf(fmt, __VA_ARGS__)
#define s_char signed char

#define sCast(type, x) static_cast<type>(x)
#define rCast(type, x) reinterpret_cast<type>(x)
#define rCast_ref(type, x) reinterpret_cast<type &>(x)
#define cCast_ref(type, x) const_cast<type &>(x)
#define cCast(type, x) const_cast<type>(x)

#define FREE_POINTER(q) if(q){ \
    free(q);  \
    q = nullptr; \
}
#define DESTROY_POINTER(q) if(q){ \
    delete q;  \
    q = nullptr; \
}

//SIMPLE_DEMO only used simple
#ifdef ANDROID
    #ifdef SIMPLE_DEMO
    #else
        #ifndef USE_NATIVE_ACTIVITY
        #define USE_NATIVE_ACTIVITY 1
        #endif
    #endif
#endif

#ifndef NULL
#define NULL 0
#endif

#define ATTR_NEED_NULL __attribute__((sentinel))

#define TO_STR(a)  #a

namespace h7{
    int unsign_shift_right(int val, int bits);
    int unsign_shift_left(int val, int bits);
}


#endif //FFMPEGOPENGLDEMO_VIDEOCONFIG_H