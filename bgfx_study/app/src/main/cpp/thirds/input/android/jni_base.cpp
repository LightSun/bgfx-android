//
// Created by Administrator on 2020/11/26 0026.
//

#include "jni_base.h"


JNIEnv* h7::ensureJniEnv(){
    auto pEnv = getJNIEnv();
    if(pEnv == nullptr){
        pEnv = attachJNIEnv();
    }
    return pEnv;
}