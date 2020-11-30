//
// Created by Administrator on 2020/11/30 0030.
//

#ifndef BGFX_STUDY_SCENEHELPER_H
#define BGFX_STUDY_SCENEHELPER_H

#include "lua/SkRefCnt.h"

#define DEFINE_GET_WEAK_REF_METHOD(prefix, type, in) \
type* prefix##::get##type(){\
if(in == nullptr) return nullptr; \
in->weak_ref();\
type* result; \
if(in->try_ref()){\
result = in;\
in->unref();\
}else{\
    result = nullptr;\
}\
in->weak_unref();\
return result;\
}

#define DEFINE_GET_WEAK_REF_METHOD2(prefix, type, name,in) \
type* prefix##::get##name(){\
if(in == nullptr) return nullptr; \
in->weak_ref();\
type* result; \
if(in->try_ref()){\
result = in;\
in->unref();\
}else{\
    result = nullptr;\
}\
in->weak_unref();\
return result;\
}

#endif //BGFX_STUDY_SCENEHELPER_H
