//
// Created by Administrator on 2020/11/28 0028.
//

#ifndef BGFX_STUDY_GROUP_H
#define BGFX_STUDY_GROUP_H

#include "lua/SkWeakRefCnt.h"
#include "Actor.h"

namespace h7{

    class Group: public Actor, public SkWeakRefCnt{
    public:
        sk_sp<Group> parent;
    };
}

#endif //BGFX_STUDY_GROUP_H
