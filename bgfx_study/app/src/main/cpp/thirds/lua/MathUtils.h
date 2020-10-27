//
// Created by Administrator on 2020/10/8 0008.
//

#ifndef BGFX_STUDY_MATHUTILS_H
#define BGFX_STUDY_MATHUTILS_H

#include <functional>

class MathUtils{
public:
    //typedef void (*SetValue)(int id1, int id2, double value);
   // typedef double (*GetValue)(int id1, int id2);
    /**
     * inverse a matrix.
     * @param n the n-order matrix
     * @param func_set the function to set
     * @param func_get the function to get
     * @return 1 if success. 0 for failed(if can't inverse).
     */
    static int inverse(int n, const std::function<void(int,int,double)>& func_set,const std::function<double(int,int)>& func_get);

    static bool doubleEqual(double x1, double x2);
};


#endif //BGFX_STUDY_MATHUTILS_H
