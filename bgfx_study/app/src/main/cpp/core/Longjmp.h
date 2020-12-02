//
// Created by Administrator on 2020/12/2 0002.
//

#ifndef BGFX_STUDY_LONGJMP_H
#define BGFX_STUDY_LONGJMP_H

#include <stdio.h>
#include <setjmp.h>

#define VTC_TYPE_NONE -1
#define VTC_TYPE_FLOAT 1
#define VTC_TYPE_DOUBLE 2
#define VTC_TYPE_LONG_LONG 3
#define VTC_TYPE_U_LONG_LONG 4
#define VTC_TYPE_INT 5

namespace h7{

    typedef union Value{
        float f;
        double d;
        long long l;
        int i;
        unsigned long long ul;
        void* ptr;
    } Value;

    typedef struct Result{
        Value val;
        short type;
    } Result;

    typedef void (*TC_FUNC)(void* ud, bool* jumpResult, Result& out);
    typedef void (*TC_FUNC_TRUE)(void* ud, Result& in, Result& out);
    typedef void (*TC_FUNC_FALSE)(void* ud, Result& in, Result& out);

    class Longjmp{
    private:
        jmp_buf buf;
        TC_FUNC_FALSE func_false;
        TC_FUNC_TRUE func_true;
        TC_FUNC func;
        Result in;

    public:
        /**
         *
         * @param func the func called first after jump
         * @param func_true the true function when continue, caused by 'longjmp(buf, 1).'
         * @param func_false function to called when no need longjmp.
         */
        Longjmp(TC_FUNC func, TC_FUNC_TRUE func_true, TC_FUNC_FALSE func_false = NULL);
        /**
         * run this action with ud and result
         * @param ud
         * @param result
         * @return true if jump result ok.
         */
        bool run(void* ud, Result& result);
    };
}
#endif //BGFX_STUDY_LONGJMP_H
