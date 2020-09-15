//
// Created by Administrator on 2020/7/3 0003.
//

#ifndef FFMPEGOPENGLDEMO_VIDEOCONFIG_H
#define FFMPEGOPENGLDEMO_VIDEOCONFIG_H

#include "bx/debug.h"

#define SkASSERT(cond) static_cast<void>(0)
#define SkDebugf(fmt, ...)  bx::debugPrintf(fmt, __VA_ARGS__)
#define s_char signed char


#define FREE_POINTER(q) if(q){ \
    free(q);  \
    q = nullptr; \
}
#define DESTROY_POINTER(q) if(q){ \
    delete q;  \
    q = nullptr; \
}

#define DESTROY_POINTER_ARRAY(array) \
if(array){ \
for (int i = 0; i < count; ++i) { \
auto pMemory = array[i]; \
if(pMemory != nullptr){ \
delete pMemory; \
} \
} \
delete[](array); \
array = nullptr; \
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
/**
 * @param str the full string
 * @param s the delimiter
 * @return the suffix string
 */
const char* getSuffixStr(const char* str, const char* s);

#define getEnumName(x) getSuffixStr(#x, ":")


#endif //FFMPEGOPENGLDEMO_VIDEOCONFIG_H