//
// Created by Administrator on 2020/11/24 0024.
//

#ifndef BGFX_STUDY_WEAKOBJECTM_HPP
#define BGFX_STUDY_WEAKOBJECTM_HPP

#include "jni.h"
#include "../../luaext_java/java_env.h"
#include "log.h"

namespace h7{
    class WeakObjectM {
    private:
        jweak weak;
    public:
        ~WeakObjectM(){
            deleteWeakObject();
        }
        /** set the object to weak reference */
        void setRefObject(jobject obj) {
            weak = getJNIEnv()->NewWeakGlobalRef(obj);
        }
        /**
         * after call this you should call env->DeleteLocalRef().
         * */
        jobject getRefObject() {
            if (weak == NULL) {
                return NULL;
            }
            return getJNIEnv()->NewLocalRef(weak);
        }

        void deleteWeakObject() {
            if (weak != NULL) {
                getJNIEnv()->DeleteWeakGlobalRef(weak);
                weak = NULL;
            }
        }
    };
}

#endif //BGFX_STUDY_WEAKOBJECTM_HPP
