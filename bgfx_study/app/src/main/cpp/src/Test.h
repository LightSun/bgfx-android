//
// Created by Administrator on 2020/12/2 0002.
//

#ifndef BGFX_STUDY_TEST_H
#define BGFX_STUDY_TEST_H

#include "utils/Array.h"
#include "scene/Viewport.h"
#include "lua/SkRefCnt.h"

namespace h7{

    template <typename T>
    class SpTraveller: public ArrayIterator<sk_sp<T>>{
    public:
        bool iterate(Array<sk_sp<T>>* arr, int index, sk_sp<T>& ele) override {
            ele.reset();
            return false;
        }
    };
    class SpTraveller_vp: public SpTraveller<Viewport>{

    };
    void _testArray();
}

#endif //BGFX_STUDY_TEST_H
