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
#include "EventListener.h"
#include "lua/SkWeakRefCnt.h"

#ifndef PI
#define PI 3.14159265358979323846264338327
#endif

namespace h7 {

    void Actor::act(float delta) {
        if (actions.size() == 0) return;

        if (stage != NULL){
            stage->weak_ref();
            if(stage->try_ref()){
                if (stage->getActionsRequestRendering()) {
                    //TODO requestRendering
                }
                stage->unref();
            } else{
                // in the disposed state.
                LOGE("stage is in the disposed state");
            }
            stage->weak_unref();
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
        sk_sp<Group> _parent = this->parent;
        while (_parent.get() != NULL) {
            ascendants.add(_parent);
            _parent = _parent->parent;
        }

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
}