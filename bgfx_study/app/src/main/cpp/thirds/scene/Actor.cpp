//
// Created by Administrator on 2020/11/27 0027.
//
#include <math.h>
#include <log.h>
#include "bx/math.h"
#include "Actor.h"
#include "Stage.h"
#include "Action.h"
#include "Group.h"
#include "Event.h"
#include "EventListener.h"
#include "lua/SkWeakRefCnt.h"
#include "../input/GestureContext.h"

#include "SceneHelper.h"

namespace h7 {
    Stage *Actor::getStage() {
        if (stage == nullptr)return nullptr;
        stage->weak_ref();
        Stage *result;
        if (stage->try_ref()) {
            result = stage;
            stage->unref();
        }
        else { result = nullptr; }
        stage->weak_unref();
        return result;
    }
    Group *Actor::getParent() {
        if (parent == nullptr)return nullptr;
        parent->weak_ref();
        Group *result;
        if (parent->try_ref()) {
            result = parent;
            parent->unref();
        }
        else { result = nullptr; }
        parent->weak_unref();
        return result;
    }
   // DEFINE_GET_WEAK_REF_METHOD2(Actor::, Group, Parent, parent)

    void Actor::act(float delta) {
        if (actions.size() == 0) return;

        auto pStage = getStage();
        if (pStage != NULL){
            if (pStage->getActionsRequestRendering()) {
                requestRender();
            }
        }
        for (int i = 0; i < actions.size(); i++) {
            sk_sp<Action> action = actions.get(i);
            if (action->act(delta) && i < actions.size()) {
                sk_sp<Action> current = actions.get(i);
                int actionIndex = current == action ? i : actions.indexOf(action);
                if (actionIndex != -1) {
                    actions.removeAt(actionIndex);
                    action->setActor(NULL);
                    i--;
                }
            }
        }
    }

    bool Actor::fire(Event &event) {
        if (event.getStage() == NULL){
            event.setStage(stage);
        }
        event.setTarget(this);

        // Collect ascendants so event propagation is unaffected by hierarchy changes.
        Array<sk_sp<Group>> ascendants;
        parent->weak_ref();
        if(parent->try_ref()){
            Group* _parent = this->parent;
            while (_parent != NULL) {
                sk_sp<Group> sp_parent = sk_ref_sp(_parent);
                ascendants.add(sp_parent);
                //
                auto pGroup = _parent->parent;
                pGroup->weak_ref();
                if(pGroup->try_ref()){
                    _parent = pGroup;
                    pGroup->unref();
                }
                pGroup->weak_unref();
            }
            parent->unref();
        }
        parent->weak_unref();

        // Notify ascendants' capture listeners, starting at the root. Ascendants may stop an event before children receive it.
        // Array<sk_sp<Group>>& ascendantsArray = ascendants.copy();
        for (int i = ascendants.size() - 1; i >= 0; i--) {
            sk_sp<Group> currentTarget = ascendants[i];
            currentTarget->notify(event, true);
            if (event.isStopped()) return event.isCancelled();
        }

        // Notify the target capture listeners.
        notify(event, true);
        if (event.isStopped()) return event.isCancelled();

        // Notify the target listeners.
        notify(event, false);
        if (!event.getBubbles()) return event.isCancelled();
        if (event.isStopped()) return event.isCancelled();

        // Notify ascendants' actor listeners, starting at the target. Children may stop an event before ascendants receive it.
        for (int i = 0, n = ascendants.size(); i < n; i++) {
            ascendants[i]->notify(event, false);
            if (event.isStopped()) return event.isCancelled();
        }
        return event.isCancelled();
    }

    bool Actor::notify(Event &event, bool capture) {
        if (event.getTarget() == NULL) {
            return false;
        }

        Array<sk_sp<EventListener>> &listeners = capture ? captureListeners : this->listeners;
        if (listeners.size() == 0) return event.isCancelled();

        event.setListenerActor(this);
        event.setCapture(capture);
        if (event.getStage() == NULL) event.setStage(stage);

        auto array = listeners.copy();
        for (int i = 0, n = array.size(); i < n; i++) {
            if (array[i]->handle(event)) {
                event.handle();
            }
        }
        return event.isCancelled();
    }

    sk_sp<Actor> Actor::hit(float x, float y, bool touchable) {
        if (touchable && this->touchable != Touchable::enabled) return NULL;
        if (!isVisible()) return NULL;
        return sk_ref_sp<Actor>(x >= 0 && x < width && y >= 0 && y < height ? this : NULL);
    }

    Vector2f &Actor::screenToLocalCoordinates(Vector2f &screenCoords) {
        if (stage == nullptr) return screenCoords;
        return stageToLocalCoordinates(stage->screenToStageCoordinates(screenCoords));
    }

    Vector2f &Actor::stageToLocalCoordinates(Vector2f &stageCoords) {
        if (parent != nullptr){
            parent->weak_ref();
            if(parent->try_ref()){
                parent->stageToLocalCoordinates(stageCoords);
                parent->unref();
            }
            parent->weak_unref();
        }

        parentToLocalCoordinates(stageCoords);
        return stageCoords;
    }

    Vector2f &Actor::localToParentCoordinates(h7::Vector2f &localCoords) {
        float rotation = -this->rotation;
        float x = this->x + transX;
        float y = this->y + transY;
        if (rotation == 0) {
            if (scaleX == 1 && scaleY == 1) {
                localCoords.x += x;
                localCoords.y += y;
            } else {
                float originX = this->x;
                float originY = this->y;
                localCoords.x = (localCoords.x - originX) * scaleX + originX + x;
                localCoords.y = (localCoords.y - originY) * scaleY + originY + y;
            }
        } else {
            float cos = (float) ::cos(bx::toRad(rotation));
            float sin = (float) ::sin(bx::toRad(rotation));
            float originX = this->x;
            float originY = this->y;
            float tox = (localCoords.x - originX) * scaleX;
            float toy = (localCoords.y - originY) * scaleY;
            localCoords.x = (tox * cos + toy * sin) + originX + x;
            localCoords.y = (tox * -sin + toy * cos) + originY + y;
        }
        return localCoords;
    }
    Vector2f & Actor::parentToLocalCoordinates(Vector2f &parentCoords) {
        float childX = this->x + transX;
        float childY = this->y + transY;
        if (rotation == 0) {
            if (scaleX == 1 && scaleY == 1) {
                parentCoords.x -= childX;
                parentCoords.y -= childY;
            } else {
                float originX = this->x;
                float originY = this->y;
                parentCoords.x = (parentCoords.x - childX - originX) / scaleX + originX;
                parentCoords.y = (parentCoords.y - childY - originY) / scaleY + originY;
            }
        } else {
            float cos = (float)::cos(bx::toRad(rotation));
            float sin = (float)::sin(bx::toRad(rotation));
            float originX = this->x;
            float originY = this->y;
            float tox = parentCoords.x - childX - originX;
            float toy = parentCoords.y - childY - originY;
            parentCoords.x = (tox * cos + toy * sin) / scaleX + originX;
            parentCoords.y = (tox * -sin + toy * cos) / scaleY + originY;
        }
        return parentCoords;
    }
    bool Actor::removeFromParent() {
        auto pGroup = getParent();
        if(pGroup != nullptr){
            return pGroup->removeActor(this, true);
        }
        return false;
    }

    void Actor::clearActions() {
        auto func = [&](Array<sk_sp<Action>>* arr, int index, sk_sp<Action>& ele){
            ele->setActor(nullptr);
            ele.reset();
            return false;
        };
        actions.clear(func);
    }
    void Actor::clearListeners() {
        auto func = [&](Array<sk_sp<EventListener>>* arr, int index, sk_sp<EventListener>& ele){
            ele.reset();
            return false;
        };
        listeners.clear(func);
        captureListeners.clear(func);
    }
    bool Actor::isDescendantOf(Actor* actor) {
        if(actor != nullptr){
            Actor* _parent = this;
            do {
                if (_parent == actor) return true;
                _parent = _parent->getParent();
            } while (_parent != nullptr);
        }
        return false;
    }
    bool Actor::isAscendantOf(Actor* actor) {
        if(actor != nullptr){
            Actor* act = actor;
            do {
                if (act == this) return true;
                act = act->getParent();
            } while (act != nullptr);
        }
        return false;
    }
    bool Actor::hasKeyboardFocus() {
        Stage* stage = getStage();
        return stage != nullptr && stage->getKeyboardFocus().get() == this;
    }
    bool Actor::hasScrollFocus() {
        Stage* stage = getStage();
        return stage != nullptr && stage->getScrollFocus().get() == this;
    }
    bool Actor::isTouchFocusTarget() {
        Stage* stage = getStage();
        if (stage == nullptr) return false;
        for (int i = 0, n = stage->touchFocuses.size(); i < n; i++)
            if (stage->touchFocuses.get(i)->target.get() == this) return true;
        return false;
    }
     bool Actor::isTouchFocusListener () {
         Stage* stage = getStage();
         if (stage == nullptr) return false;
         for (int i = 0, n = stage->touchFocuses.size(); i < n; i++)
             if (stage->touchFocuses.get(i)->listenerActor.get() == this) return true;
         return false;
    }
    bool Actor::setZIndex(int index) {
        if (index < 0) return false;
        Group* parent = getParent();
        if (parent == nullptr) return false;
        Array<sk_sp<Actor>> children = parent->children;
        if (children.size() == 1) return false;
        index = bx::min(index, children.size() - 1);
        if (children.get(index).get() == this) return false;

        auto sp = sk_ref_sp<Actor>(this);
        if (!children.remove(sp)) return false;
        children.add(index, sp);
        return true;
    }

    int Actor::getZIndex() {
        Group* parent = getParent();
        if (parent == nullptr) return -1;
        auto sp = sk_ref_sp<Actor>(this);
        return parent->children.indexOf(sp);
    }
}