//
// Created by Administrator on 2020/12/25 0025.
//

#ifndef BGFX_STUDY_LINEARLAYOUT_H
#define BGFX_STUDY_LINEARLAYOUT_H

#include "../WidgetGroup.h"
#include "../../lua/SkRefCnt.h"

namespace h7{

    class LinearLayout: public WidgetGroup{

        class WeightProvider;

    private:
        bool vertical = true;
        sk_sp<WeightProvider> weightProvider;

    public:
        class WeightProvider{
        public:
            virtual int getWeight(LinearLayout& layout, int index){
                return 1;
            }
        };

        virtual void onLayoutChildren(float targetX, float targetY, float w, float h);

        bool isVertical() const;
        void setVertical(bool vertical) ;
        WeightProvider* getWeightProvider();
        void setWeightProvider(WeightProvider* provider);
    };
}

#endif //BGFX_STUDY_LINEARLAYOUT_H
