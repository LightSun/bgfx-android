//
// Created by Administrator on 2020/12/4 0004.
//

#include "Matrix3f.h"
#include "Matrix4f.h"

namespace h7{

    void Matrix3f::toString(SB::StringBuilder &sb) {
        SB::appendArray(sb, mMat, 9);
    }

    void Matrix4f::toString(SB::StringBuilder &sb) {
        SB::appendArray(sb, mMat, 16);
    }
}