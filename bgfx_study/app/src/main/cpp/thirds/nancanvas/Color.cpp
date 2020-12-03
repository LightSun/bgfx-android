//
// Created by Administrator on 2020/12/3 0003.
//

#include "Color.h"

namespace h7{
    static inline int clamp(int com) {
        if (com < 0) {
            com = 0;
        }
        if (com > 255) {
            com = 255;
        }
        return com;
    }

    Byte h7_COLOR_CAST(float a){
        return static_cast<Byte>(clamp(static_cast<int>(a)));
    }
}