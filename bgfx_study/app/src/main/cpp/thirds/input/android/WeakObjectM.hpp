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
        inline void setRefObject(jobject obj) {
            weak = getJNIEnv()->NewWeakGlobalRef(obj);
        }
        /**
         * after call this you should call env->DeleteLocalRef().
         * */
        inline jobject getRefObject() {
            if (weak == NULL) {
                return NULL;
            }
            return getJNIEnv()->NewLocalRef(weak);
        }

        inline void deleteWeakObject() {
            if (weak != NULL) {
                getJNIEnv()->DeleteWeakGlobalRef(weak);
                weak = NULL;
            }
        }
    };

    class GlobalObjectM{
    private:
        jobject ref;
    public:
        /** set the object to weak reference */
        inline void setRefObject(jobject obj) {
            ref = getJNIEnv()->NewGlobalRef(obj);
        }
        /**
         * after call this you should call env->DeleteLocalRef().
         * */
        inline jobject getRefObject() {
            if (ref == NULL) {
                return NULL;
            }
            return getJNIEnv()->NewLocalRef(ref);
        }

        inline void deleteRefObject() {
            if (ref != NULL) {
                getJNIEnv()->DeleteGlobalRef(ref);
                ref = NULL;
            }
        }
    };
}

#endif //BGFX_STUDY_WEAKOBJECTM_HPP
