//
// Created by Administrator on 2020/10/25 0025.
//

#include "HMath.h"
#include "MathUtils.h"

namespace HMath{
    LinearEquation::LinearEquation(double a, double b) {
        this->a = a;
        this->b = b;
    }
    LinearEquation::LinearEquation(double x1, double y1, double x2, double y2) {
        if(x2 != x1){
            a = (y2 - y1)/(x2 - x1);
            b = y1 - a * x1;
        } else{
            //can't compute
            a = 0;
            b = 0;
        }
    }
    int LinearEquation::getLocation(double x, double y) {
        const auto ey = a * x + b;
        if(MathUtils::doubleEqual(ey, y)){
            return 0;
        }
        if(ey > y){
            return -1;
        }
        //ey < y
        return 1;
    }
}