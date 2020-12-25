//
// Created by Administrator on 2020/11/30 0030.
//

#include "Group.h"
#include "Actor.h"
#include "Stage.h"
#include "SceneHelper.h"

namespace h7{

    void Group::addActorAt(int index, Actor* actor) {
        Group* _tparent = actor->getParent();
        if (_tparent != nullptr) {
            if (_tparent == this) return;
            _tparent->removeActor(actor, false);
        }

        auto sp = sk_ref_sp(actor);
        if (index >= children.size())
            children.add(sp);
        else
            children.add(index, sp);
        actor->setParent(this);
        actor->setStage(getStage());
        childrenChanged();
    }
    void Group::addActorBefore(Actor* anchor, Actor* actor) {
        Group* _tparent = actor->getParent();
        if (_tparent != nullptr) {
            if (_tparent == this) return;
            _tparent->removeActor(actor, false);
        }
        int index;
        if(anchor == nullptr){
            index = 0;
        } else{
            auto sp = sk_ref_sp(anchor);
            index = children.indexOf(sp);
        }
        if(index >= 0){
            auto sp = sk_ref_sp(actor);
            children.add(index, sp);
            actor->setParent(this);
            actor->setStage(getStage());
            childrenChanged();
        }
    }

    void Group::addActorAfter(Actor* anchor, Actor* actor) {
        Group* _tparent = actor->getParent();
        if (_tparent != nullptr) {
            if (_tparent == this) return;
            _tparent->removeActor(actor, false);
        }
        auto sp_before = sk_ref_sp(anchor);
        auto sp = sk_ref_sp(actor);
        int index = children.indexOf(sp_before);
        if(index >= 0){
            children.add(index + 1, sp);
        } else{
            children.add(sp);
        }
        actor->setParent(this);
        actor->setStage(getStage());
        childrenChanged();
    }

    bool Group::removeActor (Actor* actor, bool unfocus) {
        auto sp = sk_ref_sp(actor);
        int index = children.indexOf(sp);
        if (index == -1) return false;
        removeActorAt(index, unfocus);
        return true;
    }

    sk_sp<Actor>& Group::removeActorAt(int index, bool unfocus) {
        sk_sp<Actor> actor = children.removeAt(index);
        if (unfocus) {
            Stage* stage = getStage();
            if (stage != NULL) stage->unfocus(actor.get());
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
            child.reset();
        }
        children.clear();
        childrenChanged();
    }

    bool Group::swapActor(int first, int second) {
        int maxIndex = children.size();
        if (first < 0 || first >= maxIndex) return false;
        if (second < 0 || second >= maxIndex) return false;
        children.get(first).swap(children.get(second));
        return true;
    }
    bool Group::swapActor(Actor *first, Actor *second) {
        auto sp1 = sk_ref_sp(first);
        auto sp2 = sk_ref_sp(second);
        int firstIndex = children.indexOf(sp1);
        int secondIndex = children.indexOf(sp2);
        if (firstIndex == -1 || secondIndex == -1) return false;
        children.get(firstIndex).swap(children.get(secondIndex));
        return true;
    }
}