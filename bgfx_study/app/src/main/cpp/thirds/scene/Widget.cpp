//
// Created by Administrator on 2020/12/23 0023.
//

#include "Widget.h"
#include "SceneHelper.h"

namespace h7{

    Widget::Widget():Actor(), Layout() {

    }
    Widget::~Widget()= default;
    int Widget::getActorType(){
        return H7_ACTOR_TYPE | H7_LAYOUT_TYPE;
    }
    void Widget::doLayout(float ex, float ey, float ew, float eh) {
        if(isNeedLayout()){
            setNeedLayout(false);
            layoutSize(getWidth(), getHeight(), ew, eh);
            Actor::doLayout(ex, ey, ew, eh);
        }
    }
}