//
// Created by Administrator on 2020/11/27 0027.
//

#ifndef BGFX_STUDY_ACTOR_H
#define BGFX_STUDY_ACTOR_H

#include "limits.h"
#include <math/Vector2f.h>
#include <lua/SkWeakRefCnt.h>
#include <nancanvas/Color.h>
#include "utils/Array.h"
#include "Align.h"
#include "lua/SkFloatBits.h"

#include "../nancanvas/SkMatrix.h"
#include "ActorListenerManager.h"

namespace NanoCanvas {
    class Canvas;
}

namespace h7 {

    class Stage;

    class Group;

    class Action;

    class EventListener;

    class Event;

    class Drawable;

    enum Touchable {
        /** All touch input events will be received by the actor and any children. */
        enabled = 0,
        /** No touch input events will be received by the actor or any children. */
        disabled = 1,
        /** No touch input events will be received by the actor, but children will still receive events. Note that events on the
         * children will still bubble to the parent. */
        childrenOnly = 2
    };
    /**
     * the actor : (0, 0) is at the left-top.
     */
    class Actor : public SkWeakRefCnt {
    private:
        sk_sp<Stage> stage;
        sk_sp<Group> parent;
        bool debug;
        Array<sk_sp<EventListener>> listeners;
        Array<sk_sp<EventListener>> captureListeners = Array<sk_sp<EventListener>>(0);
        Array<sk_sp<Action>> actions = Array<sk_sp<Action>>(0);
        ActorListenerManager _actorListenerM;

        SkMatrix _mat;

        const char *name = "h7_Actor";
        bool visible = true;
        Color color;
        sk_sp<Drawable> background;
        void *userObject;

        float x, y;  // relative to parent.
        float width, height;

        float scaleX = 1, scaleY = 1;
        float rotation = 0;
        float originX = 0, originY = 0;   // rotate center. like android PivotX/PivotY. often is transX/trnasY
        //float skewX = 0, skewY = 0;       // horizontal/vertical skew factor
    public:
        Touchable touchable = Touchable::enabled;

        class Iterator : public ArrayIterator<sk_sp<Actor>> {
        public:
            bool iterate(Array <sk_sp<Actor>> *arr, int index, sk_sp<Actor> &ele) {
                auto pActor = ele.get();
                pActor->_actorListenerM.setActor(nullptr);
                ele.reset();
                return false;
            }
        };
        Actor();

        void setBackground(Drawable *d);
        Drawable* getBackground();

        virtual bool isGroup(){
            return false;
        }
        inline void setDebug(bool debug){
            this->debug = debug;
        }
        inline bool isDebug(){
            return debug;
        }
        inline void setParent(Group *_g) {
            this->parent.reset(_g);
        }
        virtual void setStage(Stage *_g) {
            this->stage.reset(_g);
        }

        Stage *getStage();

        Group *getParent();

        /** Draws the actor. The batch is configured to draw in the parent's coordinate system.
	 * {@link Batch#draw(com.badlogic.gdx.graphics.g2d.TextureRegion, float, float, float, float, float, float, float, float, float)
	 * This draw method} is convenient to draw a rotated and scaled TextureRegion. {@link Batch#begin()} has already been called on
	 * the batch. If {@link Batch#end()} is called to draw without the batch then {@link Batch#begin()} must be called before the
	 * method returns.
	 * <p>
	 * The default implementation does nothing.
	 * @param parentAlpha The parent alpha, to be multiplied with this actor's alpha, allowing the parent's alpha to affect all
	 *           children. */
        virtual void draw(NanoCanvas::Canvas &canvas, float parentAlpha);

    protected:
        virtual void onDraw(NanoCanvas::Canvas &canvas, float parentAlpha) {

        }

    public:
        /** Updates the actor based on time. Typically this is called each frame by {@link Stage#act(float)}.
         * <p>
         * The default implementation calls {@link Action#act(float)} on each action and removes actions that are complete.
         * @param delta Time in seconds since the last frame. */
        virtual void act(float delta);

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
        const Actor* hit(float x, float y, bool touchable);

        inline SkMatrix& getMatrix(){
            return _mat;
        }
        /** Transforms the specified point in screen coordinates to the actor's local coordinate system.
 * @see Stage#screenToStageCoordinates(Vector2) */
        Vector2f &screenToLocalCoordinates(Vector2f &vec);

        Vector2f &localToScreenCoordinates(Vector2f &vec);

        /** Transforms the specified point in the stage's coordinates to the actor's local coordinate system. */
        Vector2f &stageToLocalCoordinates(Vector2f &vec);

        Vector2f &localCoordinatesToStage(Vector2f &vec);

        /** Transforms the specified point in the actor's coordinates to be in the parent's coordinates. */
        Vector2f &localToParentCoordinates(Vector2f &vec);

        /** Converts the coordinates given in the parent's coordinate system to this actor's coordinate system. */
        Vector2f &parentToLocalCoordinates(Vector2f &vec);

        inline void addAction(Action* action) {
            if(action == NULL)
                return ;
            auto sp = sk_ref_sp(action);
            actions.add(sp);
        }
        inline void removeAction(Action* action) {
            if(action == NULL)
                return ;
            auto sp = sk_ref_sp(action);
            actions.remove(sp);
        }
        inline Array<sk_sp<Action>>& getActions() {
            return actions;
        }
        inline bool hasActions() {
            return actions.size() > 0;
        }
        bool removeFromParent();
        /** Add a listener to receive events that {@link #hit(float, float, boolean) hit} this actor. See {@link #fire(Event)}.
	 * @see InputListener
	 * @see ClickListener */
        bool addListener(EventListener* listener){
            if(listener == NULL)
                return false;
            auto sp = sk_ref_sp(listener);
            return listeners.add(sp);
        }

        bool removeListener(EventListener* listener) {
            if(listener == NULL)
                return false;
            auto sp = sk_ref_sp(listener);
            return listeners.remove(sp);
        }
/** Adds a listener that is only notified during the capture phase.
	 * @see #fire(Event) */
        bool addCaptureListener(EventListener* listener) {
            if(listener == NULL)
                return false;
            auto sp = sk_ref_sp(listener);
            return captureListeners.add(sp);
        }
        bool removeCaptureListener(EventListener* listener) {
            if(listener == NULL)
                return false;
            auto sp = sk_ref_sp(listener);
            return captureListeners.remove(sp);
        }

        void clearActions();

        void clearListeners();

/** Removes all actions and listeners on this actor. */
        virtual void clear() {
            clearActions();
            clearListeners();
        }

        /** Returns true if this actor is the same as or is the descendant of the specified actor. */
        bool isDescendantOf(Actor *actor);

/** Returns true if this actor is the same as or is the ascendant of the specified actor. */
        bool isAscendantOf(Actor *actor);

        /** Returns true if input events are processed by this actor. */
        inline bool isTouchable() {
            return touchable == Touchable::enabled;
        }

        /** Returns true if this actor is the {@link Stage#getKeyboardFocus() keyboard focus} actor. */
        bool hasKeyboardFocus();

        /** Returns true if this actor is the {@link Stage#getScrollFocus() scroll focus} actor. */
        bool hasScrollFocus();

        /** Returns true if this actor is a target actor for touch focus.
	 * @see Stage#addTouchFocus(EventListener, Actor, Actor, int, int) */
        bool isTouchFocusTarget();

        /** Returns true if this actor is a listener actor for touch focus.
    * @see Stage#addTouchFocus(EventListener, Actor, Actor, int, int) */
        bool isTouchFocusListener();

        /** Returns true if this actor and all ascendants are visible. */
        inline bool ascendantsVisible () {
            Actor* actor = this;
            do {
                if (!actor->isVisible()) return false;
                actor = reinterpret_cast<Actor *>(actor->getParent());
            } while (actor != nullptr);
            return true;
        }
        //--------------------------------------
        inline void *getUserObject() {
            return userObject;
        }

        inline void setUserObject(void *_userObject) {
            this->userObject = _userObject;
        }
        inline bool isVisible () {
            return visible;
        }
        /** If false, the actor will not be drawn and will not receive touch events. Default is true. */
        inline void setVisible(bool _visible) {
            this->visible = _visible;
        }
        /** Sets the width and height. */
        inline void setSize (float width, float height) {
            if (this->width != width || this->height != height) {
                this->width = width;
                this->height = height;
                sizeChanged();
            }
        }
        /** Returns the X position of the actor's left edge. */
        inline float getX() {
            return x;
        }
        /** Returns the X position of the specified {@link Align alignment}. */
        inline float getX(int alignment) {
            float x = this->x;
            if ((alignment & Align::right) != 0)
                x += width;
            else if ((alignment & Align::center) != 0) //
                x += width / 2;
            return x;
        }
        inline void setX(float x) {
            if (this->x != x) {
                this->x = x;
                positionChanged();
            }
        }
        /** Returns the Y position of the actor's bottom edge. */
        inline float getY() {
            return y;
        }
        inline void setY(float y) {
            if (this->y != y) {
                this->y = y;
                positionChanged();
            }
        }
        /** Sets the y position using the specified {@link Align alignment}. Note this may set the position to non-integer
         * coordinates. */
        inline void setY(float y, int alignment) {
            if ((alignment & Align::bottom) != 0)
                y -= height;
            else if ((alignment & Align::center) != 0)
                y -= height / 2;

            if (this->y != y) {
                this->y = y;
                positionChanged();
            }
        }

        /** Returns the Y position of the specified {@link Align alignment}. */
        inline float getY(int alignment) {
            float y = this->y;
            if ((alignment & Align::bottom) != 0)
                y += height;
            else if ((alignment & Align::center) != 0)
                y += height / 2;
            return y;
        }
        /** Sets the position of the actor's bottom left corner. */
        inline void setPosition (float x, float y) {
            if (this->x != x || this->y != y) {
                this->x = x;
                this->y = y;
                positionChanged();
            }
        }

        /** Sets the position using the specified {@link Align alignment}. Note this may set the position to non-integer
         * coordinates. */
        inline void setPosition(float x, float y, int alignment) {
            if ((alignment & Align::right) != 0)
                x -= width;
            else if ((alignment & Align::center) != 0)
                x -= width / 2;

            if ((alignment & Align::bottom) != 0)
                y -= height;
            else if ((alignment & Align::center) != 0)
                y -= height / 2;
            if (this->x != x || this->y != y) {
                this->x = x;
                this->y = y;
                positionChanged();
            }
        }
        /** Add x and y to current position */
        inline void moveBy(float x, float y) {
            if (x != 0 || y != 0) {
                this->x += x;
                this->y += y;
                positionChanged();
            }
        }
        inline float getWidth() {
            return width;
        }
        inline void setWidth(float width) {
            if (this->width != width) {
                this->width = width;
                sizeChanged();
            }
        }
        inline float getHeight () {
            return height;
        }
        inline void setHeight (float height) {
            if (this->height != height) {
                this->height = height;
                sizeChanged();
            }
        }
        /** Returns x plus width. */
        inline float getRight () {
            return x + width;
        }
        inline float getBottom(){
            return y + height;
        }
        inline float getTop(){
            return y;
        }
        inline float getLeft(){
            return x;
        }
        inline void getBounds(float out[4]){
            out[0] = getLeft();
            out[1] = getTop();
            out[2] = getRight();
            out[3] = getBottom();
        }
        /** default is center */
        inline float getOriginX () {
            return originX;
        }
        /** default is center */
        inline float getOriginY () {
            return originY;
        }
        /** Sets the origin position which is relative to the actor's bottom left corner. */
        inline void setOrigin(float originX, float originY) {
            if(this->originX != originX || this->originY != originY){
                this->originX = originX;
                this->originY = originY;
                translateChanged();
            }
        }
        /** Sets the origin position to the specified {@link Align alignment}. */
        inline void setOrigin(int alignment) {
            if ((alignment & Align::left) != 0)
                originX = 0;
            else if ((alignment & Align::right) != 0)
                originX = width;
            else
                originX = width / 2;

            if ((alignment & Align::bottom) != 0)
                originY = 0;
            else if ((alignment & Align::top) != 0)
                originY = height;
            else
                originY = height / 2;
            translateChanged();
        }
        inline float getScaleX() {
            return scaleX;
        }
        inline float getScaleY() {
            return scaleY;
        }
        /** Sets the scale for both X and Y */
        inline void setScale(float scaleXY) {
            setScale(scaleXY, scaleXY);
        }

        /** Sets the scale X and scale Y. */
        inline void setScale (float scaleX, float scaleY) {
            if(this->scaleX != scaleX || this->scaleY != scaleY){
                this->scaleX = scaleX;
                this->scaleY = scaleY;
                scaleChanged();
            }
        }

        /** Adds the specified scale to the current scale. */
        inline void scaleBy (float scale) {
            scaleBy(scale, scale);
        }

        /** Adds the specified scale to the current scale. */
        inline void scaleBy (float scaleX, float scaleY) {
            if (scaleX != 0 || scaleY != 0) {
                this->scaleX += scaleX;
                this->scaleY += scaleY;
                scaleChanged();
            }
        }
        inline float getRotation () {
            return rotation;
        }

        inline void setRotation (float degrees) {
            if (this->rotation != degrees) {
                this->rotation = degrees;
                rotationChanged();
            }
        }
        /** Adds the specified rotation to the current rotation. */
        inline void rotateBy (float amountInDegrees) {
            if (amountInDegrees != 0) {
                rotation = skf_mod(rotation + amountInDegrees, 360.0f);
                rotationChanged();
            }
        }
        inline void setColor (Color& color) {
            this->color.set(color);
        }
        inline void setColor (float r, float g, float b, float a) {
            color.set(r, g, b, a);
        }
        /** Returns the color the actor will be tinted when drawn. The returned instance can be modified to change the color. */
        inline Color& getColor () {
            return color;
        }
        /** @see #setName(String)
         * @return May be null. */
        inline const char* getName () {
            return name;
        }
        /** Set the actor's name, which is used for identification convenience and by {@link #toString()}.
         * @param name May be null.
         * @see Group#findActor(String) */
        inline void setName (const char* name) {
            this->name = name;
        }
        /** Changes the z-order for this actor so it is in front of all siblings. */
        inline void toFront () {
            setZIndex(INT_MAX);
        }
        /** Changes the z-order for this actor so it is in back of all siblings. */
        inline void toBack () {
            setZIndex(0);
        }
        /** Sets the z-index of this actor. The z-index is the index into the parent's {@link Group#getChildren() children}, where a
         * lower index is below a higher index. Setting a z-index higher than the number of children will move the child to the front.
         * Setting a z-index less than zero is invalid.
         * @return true if the z-index changed. */
        bool setZIndex (int index);

        /** Returns the z-index of this actor, or -1 if the actor is not in a group.
         * @see #setZIndex(int) */
        int getZIndex ();

        ActorListenerManager& getListenerManager(){
            return _actorListenerM;
        }
        /** Called when the actor's position has been changed. */
        virtual void positionChanged() {
            _actorListenerM.fire(ActorListenerManager::TYPE_POSITION);
        }
        /** Called when the actor's size has been changed. */
        virtual void sizeChanged() {
            _actorListenerM.fire(ActorListenerManager::TYPE_SIZE);
        }
        virtual void scaleChanged() {
            applyMatrix();
            _actorListenerM.fire(ActorListenerManager::TYPE_SCALE);
        }
        virtual void rotationChanged() {
            applyMatrix();
            _actorListenerM.fire(ActorListenerManager::TYPE_ROTATION);
        }
        virtual void translateChanged(){
            applyMatrix();
            _actorListenerM.fire(ActorListenerManager::TYPE_TRANSLATE);
        }
        inline void applyMatrix(){
            _mat.setTranslate(originX, originY)
                .preRotate(rotation)
                .preScale(scaleX, scaleY);
        }
    };
}
#endif //BGFX_STUDY_ACTOR_H
