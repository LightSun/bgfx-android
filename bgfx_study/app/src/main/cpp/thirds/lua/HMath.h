//
// Created by Administrator on 2020/10/25 0025.
//

#ifndef BGFX_STUDY_HMATH_H
#define BGFX_STUDY_HMATH_H

namespace HMath{
    class LinearEquation{
    private:
        double a;
        double b;

    public:
        LinearEquation(double a, double b);
        LinearEquation(double x1, double y1, double x2, double y2);
        /**
         * get the point relative this line
         * @param x the x
         * @param y the y
         * @return 0 means equals. -1 means below, 1 means above
         */
        int getLocation(double x, double y);

        bool isIn(double x, double y){return getLocation(x, y) == 0;}
    };
}

#endif //BGFX_STUDY_HMATH_H
