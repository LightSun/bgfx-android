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
        jweak _weak;
    public:
        ~WeakObjectM(){
            deleteWeakObject();
        }
        /** set the object to weak reference */
        inline void setRefObject(jobject obj) {
            _weak = getJNIEnv()->NewWeakGlobalRef(obj);
        }
        /**
         * after call this you should call env->DeleteLocalRef().
         * */
        inline jobject getRefObject() {
            if (_weak == NULL) {
                return NULL;
            }
            return getJNIEnv()->NewLocalRef(_weak);
        }

        inline void deleteWeakObject() {
            if (_weak != NULL) {
                getJNIEnv()->DeleteWeakGlobalRef(_weak);
                _weak = NULL;
            }
        }
    };

    class GlobalObjectM{
    private:
        jobject _ref;
    public:
        /** set the object to weak reference */
        inline void setRefObject(jobject obj) {
            _ref = getJNIEnv()->NewGlobalRef(obj);
        }
        /**
         * after call this you should call env->DeleteLocalRef().
         * */
        inline jobject getRefObject() {
            if (_ref == NULL) {
                return NULL;
            }
            return getJNIEnv()->NewLocalRef(_ref);
        }

        inline void deleteRefObject() {
            if (_ref != NULL) {
                getJNIEnv()->DeleteGlobalRef(_ref);
                _ref = NULL;
            }
        }
    };
}

#endif //BGFX_STUDY_WEAKOBJECTM_HPP
