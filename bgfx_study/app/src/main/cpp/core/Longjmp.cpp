//
// Created by Administrator on 2020/12/2 0002.
//

#include "Longjmp.h"

namespace h7 {

    Longjmp::Longjmp(h7::TC_FUNC func, h7::TC_FUNC_TRUE func_true, h7::TC_FUNC_FALSE func_false) :
            func_true(func_true), func(func), func_false(func_false) {

    }

    bool Longjmp::run(void* ud, Result& result){
        int jmpret = setjmp(buf);
        if(!jmpret){
            bool jumpResult;
            func(ud, &jumpResult, result);
            in.type = result.type;
            in.val = result.val;
            if(jumpResult){
                longjmp(buf, 1);
            } else{
                if(func_false){
                    func_false(ud, in, result);
                }
                return false;
            }
        }else{
            //continue
            func_true(ud, in, result);
            return true;
        }
    }
}