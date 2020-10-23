//
// Created by Administrator on 2020/10/8 0008.
//

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "MatUtils.h"

static inline uint64_t DoubleToBaised(double x)
{
    uint64_t mask = 1ULL << 63;
    uint64_t u = *((uint64_t*)&x);
    if(u&mask) u = ~u+1;
    else u |= mask;
    return u;
}


// three methods: https://blog.csdn.net/u010551600/article/details/81504909
// this is from https://blog.csdn.net/luofl1992/article/details/7792632
int MatUtils::inverse(int n, const std::function<void(int, int, double)>& func_set,
                      const std::function<double(int, int)>& func_get) {
    int *is = new int[n];
    int *js = new int[n];
    int i, j, k;
    double d, p;
    for (k = 0; k < n; k++) {
        d = 0.0;
        for (i = k; i <= n - 1; i++)
            for (j = k; j <= n - 1; j++) {
                //p = fabs(a[i][j]);
                p = fabs(func_get(i, j));
                if (p > d) {
                    d = p;
                    is[k] = i;
                    js[k] = j;
                }
            }
        if (0.0 == d) {
            free(is);
            free(js);
            //printf("can't inverse");
            return (0);
        }
        if (is[k] != k)
            for (j = 0; j <= n - 1; j++) {
                // p = a[k][j];
                // a[k][j] = a[is[k]][j];
                // a[is[k]][j]=p;
                p = func_get(k, j);
                func_set(k, j, func_get(is[k], j));
                func_set(is[k], j, p);
            }
        if (js[k] != k)
            for (i = 0; i <= n - 1; i++) {
                //p = a[i][k];
                //a[i][k] = a[i][js[k]];
                //a[i][js[k]] = p;
                p = func_get(i, k);
                func_set(i, k, func_get(i, js[k]));
                func_set(i, js[k], p);
            }
        //a[k][k] = 1.0/a[k][k];
        func_set(k, k, 1.0 / func_get(k, k));
        for (j = 0; j <= n - 1; j++)
            if (j != k) {
                //a[k][j] *= a[k][k];
                func_set(k, j, func_get(k, j) * func_get(k, k));
            }
        for (i = 0; i <= n - 1; i++)
            if (i != k)
                for (j = 0; j <= n - 1; j++)
                    if (j != k) {
                        //a[i][j] -= a[i][k] * a[k][j];
                        func_set(i, j, func_get(i, j) - func_get(i, k) * func_get(k, j));
                    }
        for (i = 0; i <= n - 1; i++)
            if (i != k) {
               // a[i][k] = -a[i][k] * a[k][k];
                func_set(i, k, -func_get(i, k) * func_get(k, k));
            }
    }
    for (k = n - 1; k >= 0; k--) {
        if (js[k] != k)
            for (j = 0; j <= n - 1; j++) {
               // p = a[k][j];
               // a[k][j] = a[js[k]][j];
               // a[js[k]][j] = p;

                p = func_get(k, j);
                func_set(k, j, func_get(js[k], j));
                func_set(js[k], j, p);
            }
        if (is[k] != k)
            for (i = 0; i <= n - 1; i++) {
               // p = a[i][k];
               // a[i][k] = a[i][is[k]];
               // a[i][is[k]] = p;

                p = func_get(i, k);
                func_set(i, k, func_get(i, is[k]));
                func_set(i, is[k], p);
            }
    }
    free(is);
    free(js);
    return 1;
}
//from google unit-test
bool MatUtils::doubleEqual(double x1, double x2) {
    uint64_t u1 = DoubleToBaised(x1);
    uint64_t u2 = DoubleToBaised(x2);
    int ulp = 4;
    if(u1>u2) return (u1-u2)<ulp;
    else return (u2-u1)<ulp;
}