//
// Created by Administrator on 2020/11/27 0027.
//

#ifndef BGFX_STUDY_ACTION_H
#define BGFX_STUDY_ACTION_H

#include <lua/SkRefCnt.h>
#include "utils/Pool.h"

namespace h7 {
    class Actor;

    class Action : public Poolable, SkRefCnt {

    protected:
        sk_sp<Actor> actor;
        sk_sp<Actor> target;
    public:
        /** Updates the action based on time. Typically this is called each frame by {@link Actor#act(float)}.
             * @param delta Time in seconds since the last frame.
             * @return true if the action is done. This method may continue to be called after the action is done. */
        virtual bool act(float delta) = 0;

        virtual void restart() {}

        /** Sets the actor this action is attached to. This also sets the {@link #setTarget(Actor) target} actor if it is null. This
	 * method is called automatically when an action is added to an actor. This method is also called with null when an action is
	 * removed from an actor.
	 * <p>
	 * When set to null, if the action has a {@link #setPool(Pool) pool} then the action is {@link Pool#free(Object) returned} to
	 * the pool (which calls {@link #reset()}) and the pool is set to null. If the action does not have a pool, {@link #reset()} is
	 * not called.
	 * <p>
	 * This method is not typically a good place for an action subclass to query the actor's state because the action may not be
	 * executed for some time, eg it may be {@link DelayAction delayed}. The actor's state is best queried in the first call to
	 * {@link #act(float)}. For a {@link TemporalAction}, use TemporalAction#begin(). */
        void setActor(sk_sp<Actor> actor);

        /** Sets the actor this action will manipulate. If no target actor is set, {@link #setActor(Actor)} will set the target actor
     * when the action is added to an actor. */
        void setTarget(sk_sp<Actor> target);

        /** @return null if the action has no target. */
        inline sk_sp<Actor> getTarget() {
            return target;
        }
        inline sk_sp<Actor> getActor() {
            return actor;
        }

        /** Resets the optional state of this action to as if it were newly created, allowing the action to be pooled and reused. State
	 * required to be set for every usage of this action or computed during the action does not need to be reset.
	 * <p>
	 * The default implementation calls {@link #restart()}.
	 * <p>
	 * If a subclass has optional state, it must override this method, call super, and reset the optional state. */
        void reset();
    };
}

#endif //BGFX_STUDY_ACTION_H
