//
// Created by Administrator on 2020/11/24 0024.
//

#ifndef BGFX_STUDY_JNI_BASE_HPP
#define BGFX_STUDY_JNI_BASE_HPP

#include "jni.h"
#include "../../luaext_java/java_env.h"
#include "log.h"

#define USE_REF_OBJECT(x) \
do{\
 auto ref = getRefObject();\
 x;\
 pEnv->DeleteLocalRef(ref);\
} while(0);


namespace h7{
    class WeakObjectM {
    private:
        jweak _weak;
    public:
        ~WeakObjectM(){
            deleteRefObject();
        }
        /** set the object to weak reference */
        virtual inline void setRefObject(jobject obj) {
            _weak = getJNIEnv()->NewWeakGlobalRef(obj);
        }
        /**
         * after call this you should call env->DeleteLocalRef().
         * */
        virtual inline jobject getRefObject() {
            if (_weak == NULL) {
                return NULL;
            }
            return getJNIEnv()->NewLocalRef(_weak);
        }

        virtual inline void deleteRefObject() {
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
        virtual inline void setRefObject(jobject obj) {
            _ref = getJNIEnv()->NewGlobalRef(obj);
        }
        /**
         * after call this you should call env->DeleteLocalRef().
         * */
        virtual inline jobject getRefObject() {
            if (_ref == NULL) {
                return NULL;
            }
            return getJNIEnv()->NewLocalRef(_ref);
        }

        virtual inline void deleteRefObject() {
            if (_ref != NULL) {
                getJNIEnv()->DeleteGlobalRef(_ref);
                _ref = NULL;
            }
        }
    };
    JNIEnv* ensureJniEnv(){
        auto pEnv = getJNIEnv();
        if(pEnv == NULL){
            pEnv = attachJNIEnv();
        }
        return pEnv;
    }
}

#endif //BGFX_STUDY_JNI_BASE_HPP
