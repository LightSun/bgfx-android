//
// Created by Administrator on 2020/11/27 0027.
//

#ifndef BGFX_STUDY_ACTOR_H
#define BGFX_STUDY_ACTOR_H

#include <lua/SkRefCnt.h>
#include <math/Vector2f.h>
#include "utils/Array.hpp"
#include "Color.h"
#include "Event.h"

namespace NanoCanvas {
    class Canvas;
}

namespace h7 {

    class Stage;

    class Group;

    class Action;

    class EventListener;

    enum Touchable {
        /** All touch input events will be received by the actor and any children. */
        enabled = 0,
        /** No touch input events will be received by the actor or any children. */
        disabled = 1,
        /** No touch input events will be received by the actor, but children will still receive events. Note that events on the
         * children will still bubble to the parent. */
        childrenOnly = 2
    };

    class Actor : public SkRefCnt {
    public:
        Stage* stage;
        Group* parent;
        Array<sk_sp<EventListener>> listeners;
        Array<sk_sp<EventListener>> captureListeners = Array<sk_sp<EventListener>>(0, NULL);
        Array<sk_sp<Action>> actions = Array<sk_sp<Action>>(0, NULL);

        const char *name;
        Touchable touchable = Touchable::enabled;
        bool visible = true;
        float x, y;
        float width, height;
        float scaleX = 1, scaleY = 1;
        float rotation = 0;
        float transX = 0, transY = 0;
        Color color;
        sk_sp<SkRefCnt> userObject;

        inline bool isVisible() {
            return visible;
        }

        /** Draws the actor. The batch is configured to draw in the parent's coordinate system.
	 * {@link Batch#draw(com.badlogic.gdx.graphics.g2d.TextureRegion, float, float, float, float, float, float, float, float, float)
	 * This draw method} is convenient to draw a rotated and scaled TextureRegion. {@link Batch#begin()} has already been called on
	 * the batch. If {@link Batch#end()} is called to draw without the batch then {@link Batch#begin()} must be called before the
	 * method returns.
	 * <p>
	 * The default implementation does nothing.
	 * @param parentAlpha The parent alpha, to be multiplied with this actor's alpha, allowing the parent's alpha to affect all
	 *           children. */
        void draw(NanoCanvas::Canvas &canvas, float parentAlpha) {
        }

        /** Updates the actor based on time. Typically this is called each frame by {@link Stage#act(float)}.
         * <p>
         * The default implementation calls {@link Action#act(float)} on each action and removes actions that are complete.
         * @param delta Time in seconds since the last frame. */
        void act(float delta);

        /** Sets this actor as the event {@link Event#setTarget(Actor) target} and propagates the event to this actor and ascendants as
	 * necessary. If this actor is not in the stage, the stage must be set before calling this method.
	 * <p>
	 * Events are fired in 2 phases:
	 * <ol>
	 * <li>The first phase (the "capture" phase) notifies listeners on each actor starting at the root and propagating down the
	 * hierarchy to (and including) this actor.</li>
	 * <li>The second phase notifies listeners on each actor starting at this actor and, if {@link Event#getBubbles()} is true,
	 * propagating upward to the root.</li>
	 * </ol>
	 * If the event is {@link Event#stop() stopped} at any time, it will not propagate to the next actor.
	 * @return true if the event was {@link Event#cancel() cancelled}. */
        bool fire(Event &event);

        /** Notifies this actor's listeners of the event. The event is not propagated to any ascendants. The event
	 * {@link Event#setTarget(Actor) target} must be set before calling this method. Before notifying the listeners, this actor is
	 * set as the {@link Event#getListenerActor() listener actor}. If this actor is not in the stage, the stage must be set before
	 * calling this method.
	 * @param capture If true, the capture listeners will be notified instead of the regular listeners.
	 * @return true if the event was {@link Event#cancel() cancelled}. */
        bool notify(Event &event, bool capture);

        /** Returns the deepest {@link #isVisible() visible} (and optionally, {@link #getTouchable() touchable}) actor that contains
       * the specified point, or null if no actor was hit. The point is specified in the actor's local coordinate system (0,0 is the
       * bottom left of the actor and width,height is the upper right).
       * <p>
       * This method is used to delegate touchDown, mouse, and enter/exit events. If this method returns null, those events will not
       * occur on this Actor.
       * <p>
       * The default implementation returns this actor if the point is within this actor's bounds and this actor is visible.
       * @param touchable If true, hit detection will respect the {@link #setTouchable(Touchable) touchability}.
       * @see Touchable */
        sk_sp<Actor> hit(float x, float y, bool touchable);

        /** Transforms the specified point in screen coordinates to the actor's local coordinate system.
 * @see Stage#screenToStageCoordinates(Vector2) */
        Vector2f &screenToLocalCoordinates(Vector2f &screenCoords);

        /** Transforms the specified point in the stage's coordinates to the actor's local coordinate system. */
        Vector2f &stageToLocalCoordinates(Vector2f &stageCoords);

        /** Transforms the specified point in the actor's coordinates to be in the parent's coordinates. */
        Vector2f& localToParentCoordinates (Vector2f& localCoords);
        /** Converts the coordinates given in the parent's coordinate system to this actor's coordinate system. */
        Vector2f& parentToLocalCoordinates (Vector2f& parentCoords);
    };
}
#endif //BGFX_STUDY_ACTOR_H