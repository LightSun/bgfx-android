//
// Created by Administrator on 2020/11/28 0028.
//

#ifndef BGFX_STUDY_GROUP_H
#define BGFX_STUDY_GROUP_H

#pragma once

#include "lua/SkWeakRefCnt.h"
#include "utils/Array.h"

namespace h7 {
    class Actor;

    class Stage;

    class Group : public Actor {
    public:
        Array<sk_sp<Actor>> children;

        void addActor(sk_sp<Actor> actor) {
            children.add(actor);
        }

        /** Adds an actor as a child of this group at a specific index, removing it from its previous parent. If the actor is already a
         * child of this group, no changes are made.
         * @param index May be greater than the number of children. */
        void addActorAt(int index, sk_sp<Actor> actor);

        /** Adds an actor as a child of this group immediately before another child actor, removing it from its previous parent. If the
         * actor is already a child of this group, no changes are made. */
        void addActorBefore(sk_sp<Actor> actorBefore, sk_sp<Actor> actor);
        /** Adds an actor as a child of this group immediately after another child actor, removing it from its previous parent. If the
	 * actor is already a child of this group, no changes are made. If <code>actorAfter</code> is not in this group, the actor is
	 * added as the last child. */
        void addActorAfter(sk_sp<Actor> actorBefore, sk_sp<Actor> actor);

        /** Removes an actor from this group. Calls {@link #removeActorAt(int, boolean)} with the actor's child index. */
        bool removeActor(sk_sp<Actor> actor, bool unfocus);

        /** Removes an actor from this group and unfocuses it. Calls {@link #removeActor(Actor, boolean)} with true. */
        bool removeActor(sk_sp<Actor> actor){
            return removeActor(actor, true);
        }
        /** Removes an actor from this group. If the actor will not be used again and has actions, they should be
 * {@link Actor#clearActions() cleared} so the actions will be returned to their
 * {@link Action#setPool(com.badlogic.gdx.utils.Pool) pool}, if any. This is not done automatically.
 * @param unfocus If true, {@link Stage#unfocus(Actor)} is called.
 * @return the actor removed from this group. */
        sk_sp<Actor> removeActorAt(int index, bool unfocus);

        /** Removes all actors from this group. */
         void clearChildren ();

        /** Called when actors are added to or removed from the group. */
        virtual void childrenChanged() {
        }
    };
}

#endif //BGFX_STUDY_GROUP_H
