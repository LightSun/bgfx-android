//
// Created by Administrator on 2020/7/3 0003.
//

#ifndef FFMPEGOPENGLDEMO_VIDEOCONFIG_H
#define FFMPEGOPENGLDEMO_VIDEOCONFIG_H

#define VC_API

//do nothing current
#define ASSERT(cond) static_cast<void>(0)
#define DEBUGCODE(cond) static_cast<void>(0)

#define CMD_QUIT  1
#define CMD_PAUSE 2

#define DESTROY_POINTER(q) if(q){ \
    delete q;  \
    q = nullptr; \
}

#define DESTROY_POINTER_FUNC(q, func_re) if(q){ \
    func_re(q); \
    delete q;  \
    q = nullptr; \
}

#define UN_REF_POINTER(p, func) if(p){ \
    func(p); \
    p = nullptr; \
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

#endif //FFMPEGOPENGLDEMO_VIDEOCONFIG_H