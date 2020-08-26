//
// Created by Administrator on 2019/7/1.
//

#ifndef COMMONLUAAPP_JAVA_ENV_H
#define COMMONLUAAPP_JAVA_ENV_H

#include <jni.h>

extern "C"{

typedef struct Registration{
    char* clazz;
    JNINativeMethod* methods;
    int len; //method count
}Registration;

Registration createRegistration(char* clazz, JNINativeMethod methods[], int len);

extern Registration getLuaStateRegistration();
extern Registration getLua2JavaRegistration();
extern void initLuaJavaCaller();
extern void deInitLuaJavaCaller();
jclass getGlobalClass(JNIEnv * env, const char* classname);

int registerMethods(JNIEnv* env,Registration n);

JNIEnv *getJNIEnv();

JNIEnv *attachJNIEnv();

void detachJNIEnv();

void dumpReferenceTables(JNIEnv *env);

const char * stringReplace(const char* str, const char* src, const char* dst);
};

#endif //COMMONLUAAPP_JAVA_ENV_H
