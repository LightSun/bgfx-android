//
// Created by Administrator on 2020/11/30 0030.
//

#include "Group.h"
#include "Actor.h"
#include "Stage.h"
#include "SceneHelper.h"

namespace h7{

    void Group::addActorAt(int index, sk_sp<Actor> actor) {
        Group* _tparent = actor->getParent();
        if (_tparent != nullptr) {
            if (_tparent == this) return;
            _tparent->removeActor(actor, false);
        }
        if (index >= children.size())
            children.add(actor);
        else
            children.add(index, actor);
        actor->setParent(this);
        actor->setStage(getStage());
        childrenChanged();
    }
    void Group::addActorBefore(sk_sp<Actor> actorBefore, sk_sp<Actor> actor) {
        Group* _tparent = actor->getParent();
        if (_tparent != nullptr) {
            if (_tparent == this) return;
            _tparent->removeActor(actor, false);
        }
        int index;
        if(actorBefore == nullptr){
            index = 0;
        } else{
            index = children.indexOf(actorBefore);
        }
        if(index >= 0){
            children.add(index, actor);
            actor->setParent(this);
            actor->setStage(getStage());
            childrenChanged();
        }
    }

    void Group::addActorAfter(sk_sp<Actor> actorBefore, sk_sp<Actor> actor) {
        Group* _tparent = actor->getParent();
        if (_tparent != nullptr) {
            if (_tparent == this) return;
            _tparent->removeActor(actor, false);
        }
        int index = children.indexOf(actorBefore);
        if(index >= 0){
            children.add(index + 1, actor);
        } else{
            children.add(actor);
        }
        actor->setParent(this);
        actor->setStage(getStage());
        childrenChanged();
    }

    bool Group::removeActor (sk_sp<Actor> actor, bool unfocus) {
        int index = children.indexOf(actor);
        if (index == -1) return false;
        removeActorAt(index, unfocus);
        return true;
    }

    sk_sp<Actor> Group::removeActorAt(int index, bool unfocus) {
        sk_sp<Actor> actor = children.removeAt(index);
        if (unfocus) {
            Stage* stage = getStage();
            if (stage != NULL) stage->unfocus(*actor);
        }
        actor->setParent(NULL);
        actor->setStage(NULL);
        childrenChanged();
        return actor;
    }
    void Group::clearChildren() {
        auto array = children.copy();
        for (int i = 0, n = array.size(); i < n; i++) {
            sk_sp<Actor> child = array[i];
            child->setStage(NULL);
            child->setParent(NULL);
        }
        auto func = [&](Array<sk_sp<Actor>>* arr, int index, sk_sp<Actor>& ele){
            ele.reset();
            return false;
        };
        children.clear(func);
        childrenChanged();
    }
}