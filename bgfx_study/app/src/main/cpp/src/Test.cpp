//
// Created by Administrator on 2020/12/2 0002.
//

#include "Test.h"
#include "log.h"

namespace h7{

    void _testArray(){
        using namespace h7;
        IntArray arr;
        ADD_BASE(&arr, 1);
        ADD_BASE(&arr, 2);
        ADD_BASE(&arr, 3);
        LOGD("arr size = %d (expect 3)", arr.size());

        Array<sk_sp<Viewport>> crr;
        auto vp = new Viewport();
        auto sp = sk_ref_sp(vp);
        crr.add(sp);
        crr.clear();
        LOGD("crr >>> Viewport ptr = %p", sp.get());

        Array<sk_sp<Viewport>>* drr = new Array<sk_sp<Viewport>>();
        drr->add(sp);
        auto travellerVp = SpTraveller_vp();
        drr->clear(&travellerVp);
        LOGD("drr >>> Viewport ptr = %p, unique = %d", sp.get(), sp->unique());
        delete drr;
        LOGD("delete drr >>> Viewport ptr = %p, unique = %d", sp.get(), sp->unique());
        vp->unref();
    }
}