//
// Created by Administrator on 2020/8/17 0017.
//

#include "global.h"
#include <iostream>

/*#define DEF_UNSIGN_SHIFT_RIGHT(type)\
type h7_unsign_shift_right(type val, int bits){\
    return (unsigned type)(val) >> bits;\
}
DEF_UNSIGN_SHIFT_RIGHT(int);*/

namespace h7{
    int unsign_shift_right(int val, int bits) {
        return ((unsigned int) (val)) >> bits;
    }
    int unsign_shift_left(int val, int bits) {
        return ((unsigned int) (val)) << bits;
    }
}
