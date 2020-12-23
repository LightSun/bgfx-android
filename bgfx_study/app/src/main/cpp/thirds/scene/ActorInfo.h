//
// Created by Administrator on 2020/12/23 0023.
//

#ifndef BGFX_STUDY_ACTORINFO_H
#define BGFX_STUDY_ACTORINFO_H

#include "../nancanvas/SkMatrix.h"

namespace h7{

    // should never use: new
    typedef struct ActorInfo{
        float x, y;
        float width, height;      // width, height
        float alpha = 1;          // alpha

        float scaleX = 1, scaleY = 1;
        float rotation = 0;   //in degree
        float transX = 0, transY = 0;
    }ActorInfo;
}

#endif //BGFX_STUDY_ACTORINFO_H
