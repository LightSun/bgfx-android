//
// Created by Administrator on 2020/12/23 0023.
//

#include "WidgetGroup.h"
#include "SceneHelper.h"

namespace h7{
    int WidgetGroup::getActorType(){
        return H7_GROUP_TYPE | H7_LAYOUT_TYPE;
    }
    void WidgetGroup::setLayoutEnabled(bool enabled) {
        layoutEnabled = enabled;
        setLayoutEnabled(this, enabled);
    }

    void WidgetGroup::setLayoutEnabled (Group* parent, bool enabled) {
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

    void WidgetGroup::validate() {
        if (!layoutEnabled) return;

        Group* parent = getParent();
        if (fillParent && parent != nullptr) {
            float parentWidth, parentHeight;
            Stage* stage = getStage();
            if (stage != nullptr && parent == stage->getRoot()) {
                parentWidth = stage->getWidth();
                parentHeight = stage->getHeight();
            } else {
                parentWidth = parent->getWidth();
                parentHeight = parent->getHeight();
            }
            if (getWidth() != parentWidth || getHeight() != parentHeight) {
                setWidth(parentWidth);
                setHeight(parentHeight);
                invalidate();
            }
        }

        if (!_needsLayout) return;
        _needsLayout = false;
        layout();

        // Widgets may call invalidateHierarchy during layout (eg, a wrapped label). The root-most widget group retries layout a
        // reasonable number of times.
        if (_needsLayout) {
            if (parent->hasActorType(H7_GROUP_TYPE | H7_LAYOUT_TYPE)) return; // The parent widget will layout again.
            for (int i = 0; i < 5; i++) {
                _needsLayout = false;
                layout();
                if (!needsLayout) break;
            }
        }
    }

    void WidgetGroup::invalidateHierarchy() {
        invalidate();
        Group* parent = getParent();
        if (parent->hasActorType(H7_LAYOUT_TYPE)){
            rCast(Layout*, parent)->invalidateHierarchy();
        }
    }

    void WidgetGroup::childrenChanged() {
        invalidateHierarchy();
    }
    void WidgetGroup::sizeChanged() {
        invalidate();
    }
    void WidgetGroup::pack() {
        setSize(getPrefWidth(), getPrefHeight());
        validate();
        // Validating the layout may change the pref size. Eg, a wrapped label doesn't know its pref height until it knows its
        // width, so it calls invalidateHierarchy() in layout() if its pref height has changed.
        setSize(getPrefWidth(), getPrefHeight());
        validate();
    }
    void WidgetGroup::draw(NanoCanvas::Canvas& canvas, float parentAlpha) {
        validate();
        Group::draw(canvas, parentAlpha);
    }
}