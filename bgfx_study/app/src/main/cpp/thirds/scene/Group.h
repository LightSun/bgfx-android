//
// Created by Administrator on 2020/11/28 0028.
//

#ifndef BGFX_STUDY_GROUP_H
#define BGFX_STUDY_GROUP_H

#pragma once

#include "lua/SkWeakRefCnt.h"
#include "utils/Array.h"
#include "Actor.h"

namespace h7 {
    class Stage;

    class Group : public Actor {
    public:
        Array<sk_sp<Actor>> children;

        bool isGroup() override {
            return true;
        }

        inline void addActor(Actor *actor) {
            auto sp = sk_ref_sp(actor);
            children.add(sp);
        }

        inline void setDebug(bool enabled, bool recursively) {
            Actor::setDebug(enabled);
            if (recursively) {
                for (int i = 0; i < children.size(); ++i) {
                    auto sp = children.get(i);
                    if (sp->isGroup()) {
                        Group *g = rCast(Group*, sp.get());
                        g->setDebug(enabled, recursively);
                    } else {
                        sp->setDebug(enabled);
                    }
                }
            }
        }
        void setStage(Stage* _s) override {
            Actor::setStage(_s);
            for (int i = 0; i < children.size(); ++i) {
                children.get(i)->setStage(_s);
            }
        }
        /** Swaps two actors by index. Returns false if the swap did not occur because the indexes were out of bounds. */
     bool swapActor (int first, int second) {
            int maxIndex = children.size;
            if (first < 0 || first >= maxIndex) return false;
            if (second < 0 || second >= maxIndex) return false;
            children.swap(first, second);
            return true;
        }

        /** Swaps two actors. Returns false if the swap did not occur because the actors are not children of this group. */
        bool swapActor (Actor first, Actor second) {
            int firstIndex = children.indexOf(first, true);
            int secondIndex = children.indexOf(second, true);
            if (firstIndex == -1 || secondIndex == -1) return false;
            children.swap(firstIndex, secondIndex);
            return true;
        }


        /** Adds an actor as a child of this group at a specific index, removing it from its previous parent. If the actor is already a
         * child of this group, no changes are made.
         * @param index May be greater than the number of children. */
        void addActorAt(int index, Actor *actor);

        /** Adds an actor as a child of this group immediately before another child actor, removing it from its previous parent. If the
         * actor is already a child of this group, no changes are made. */
        void addActorBefore(Actor *actorBefore, Actor *actor);

        /** Adds an actor as a child of this group immediately after another child actor, removing it from its previous parent. If the
	 * actor is already a child of this group, no changes are made. If <code>actorAfter</code> is not in this group, the actor is
	 * added as the last child. */
        void addActorAfter(Actor *actorBefore, Actor *actor);

        /** Removes an actor from this group. Calls {@link #removeActorAt(int, boolean)} with the actor's child index. */
        bool removeActor(Actor *actor, bool unfocus);

        /** Removes an actor from this group and unfocuses it. Calls {@link #removeActor(Actor, boolean)} with true. */
        bool removeActor(Actor *actor) {
            return removeActor(actor, true);
        }

        /** Removes an actor from this group. If the actor will not be used again and has actions, they should be
 * {@link Actor#clearActions() cleared} so the actions will be returned to their
 * {@link Action#setPool(com.badlogic.gdx.utils.Pool) pool}, if any. This is not done automatically.
 * @param unfocus If true, {@link Stage#unfocus(Actor)} is called.
 * @return the actor removed from this group. */
        sk_sp<Actor> &removeActorAt(int index, bool unfocus);

        /** Removes all actors from this group. */
        void clearChildren();

        /** Called when actors are added to or removed from the group. */
        virtual void childrenChanged() {
        }
    };
}

#endif //BGFX_STUDY_GROUP_H
