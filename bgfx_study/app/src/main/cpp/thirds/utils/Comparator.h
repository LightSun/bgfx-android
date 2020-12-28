//
// Created by Administrator on 2020/12/28 0028.
//

#ifndef BGFX_STUDY_COMPARATOR_H
#define BGFX_STUDY_COMPARATOR_H

namespace h7{

    template <typename T>
    class Comparator{
    public:
        virtual int compare(const T* o1, const T* o2){
            return 0;
        }
    };
}

#endif //BGFX_STUDY_COMPARATOR_H
