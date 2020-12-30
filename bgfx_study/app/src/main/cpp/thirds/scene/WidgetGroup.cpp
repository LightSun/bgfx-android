//
// Created by Administrator on 2020/12/23 0023.
//

#include <stdio.h>
#include <stdarg.h>
#include "WidgetGroup.h"
#include "SceneHelper.h"

namespace h7{
    WidgetGroup::WidgetGroup():Group(), Layout() {

    }
    WidgetGroup::~WidgetGroup()= default;

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
    void WidgetGroup::setNeedLayout(bool enabled) {
        //travel all child. and call setLayoutEnabled(enabled)
        Layout::setNeedLayout(enabled);
        setNeedLayout(this, enabled);
    }
    void WidgetGroup::setNeedMeasure(bool enabled) {
        Layout::setNeedMeasure(enabled);
        setNeedMeasure(this, enabled);
    }

    void WidgetGroup::setNeedLayout(Group* parent, bool enabled) {
        Array<sk_sp<Actor>> children = parent->getChildren();
        for (int i = 0, n = children.size(); i < n; i++) {
            sk_sp<Actor> actor = children.get(i);
            if(actor->hasActorType(H7_LAYOUT_TYPE)){
                ((Layout*)actor.get())->setNeedLayout(enabled);
            } else if(actor->hasActorType(H7_GROUP_TYPE)){
                setNeedLayout(((Group*)actor.get()), enabled);
            }
        }
    }
    void WidgetGroup::setNeedMeasure(Group* parent, bool enabled) {
        Array<sk_sp<Actor>> children = parent->getChildren();
        for (int i = 0, n = children.size(); i < n; i++) {
            sk_sp<Actor> actor = children.get(i);
            if(actor->hasActorType(H7_LAYOUT_TYPE)){
                ((Layout*)actor.get())->setNeedMeasure(enabled);
            } else if(actor->hasActorType(H7_GROUP_TYPE)){
                setNeedMeasure(((Group*)actor.get()), enabled);
            }
        }
    }

    void WidgetGroup::doLayout(float ex, float ey, float ew, float eh) {
        if(isNeedLayout()){
            setNeedLayout(false);
            layoutSize(getWidth(), getHeight(), ew, eh);
            Actor::doLayout(ex, ey, ew, eh);
        }
    }

    int WidgetGroup::getAlign() const {
        return align;
    }
    void WidgetGroup::setAlign(int align) {
        if(this->align != align){
            this->align = align;
            requestLayoutAndInvalidate();
        }
    }
}