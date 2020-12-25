//
// Created by Administrator on 2020/12/23 0023.
//

#include "Widget.h"
#include "SceneHelper.h"

namespace h7{

    int Widget::getActorType(){
        return H7_ACTOR_TYPE | H7_LAYOUT_TYPE;
    }
    void Widget::doLayout(float ex, float ey, float ew, float eh) {
        if(isLayoutEnabled()){
            switch (getLayoutType()){
                case LAYOUT_TYPE_MATCH_PARENT:
                    setSize(ew, eh);
                    break;
                case LAYOUT_TYPE_WRAP_CONTENT:
                    measure();
                    break;
                case LAYOUT_TYPE_REAL:
                    if(getMinWidth() > 0 && getMinHeight() > 0){
                        float w = getWidth() < getMinWidth() ? getMinWidth() : getWidth();
                        float h = getHeight() < getMinHeight() ? getMinHeight() : getHeight();
                        setSize(w, h);
                    }
                    break;
            }
            Actor::doLayout(ex, ey, ew, eh);
        }
    }
}