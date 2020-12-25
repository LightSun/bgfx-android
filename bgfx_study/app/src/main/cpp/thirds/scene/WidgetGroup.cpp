//
// Created by Administrator on 2020/12/23 0023.
//

#include <stdio.h>
#include <stdarg.h>
#include "WidgetGroup.h"
#include "SceneHelper.h"

namespace h7{
    void WidgetGroup::addActors(int n, ...) {
        va_list args;
        va_start(args, n);
        for (int i = 0; i < n; ++i) {
            addActor(va_arg(args, Actor *));
        }
        va_end(args);
    }
    int WidgetGroup::getActorType(){
        return H7_GROUP_TYPE | H7_LAYOUT_TYPE;
    }
    void WidgetGroup::setLayoutEnabled(bool enabled) {
        //travel all child. and call setLayoutEnabled(enabled)
        Layout::setLayoutEnabled(enabled);
        setLayoutEnabled(this, enabled);
    }

    void WidgetGroup::setLayoutEnabled(Group* parent, bool enabled) {
        Array<sk_sp<Actor>> children = parent->getChildren();
        for (int i = 0, n = children.size(); i < n; i++) {
            sk_sp<Actor> actor = children.get(i);
            if(actor->hasActorType(H7_LAYOUT_TYPE)){
                ((Layout*)actor.get())->setLayoutEnabled(enabled);
            } else if(actor->hasActorType(H7_GROUP_TYPE)){
                setLayoutEnabled(((Group*)actor.get()), enabled);
            }
        }
    }

    void WidgetGroup::doLayout(float ex, float ey, float ew, float eh) {
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

    int WidgetGroup::getAlign() const {
        return align;
    }
    void WidgetGroup::setAlign(int align) {
        if(this->align != align){
            this->align = align;
            layoutAndInvalidate();
        }
    }
}