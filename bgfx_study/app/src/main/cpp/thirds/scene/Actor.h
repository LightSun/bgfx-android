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

#define H7_ACTOR_TYPE 1
#define H7_GROUP_TYPE 2
#define H7_LAYOUT_TYPE 4

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
        int _flags = FLAG_VISIBLE;
        Color color;
        sk_sp<Drawable> background;
        SkRect* _padding;
        SkRect* _margin;

        void *userObject;

        float x, y;  // relative to parent.
        float width, height;
        float alpha = 1;

        float scaleX = 1, scaleY = 1;
        float rotation = 0; //in degree
        float transX = 0, transY = 0;

        float originX = -1, originY = -1;   //rotate/scale center. like android PivotX/PivotY

        float tmpScreenX, tmpScreenY;

    public:
        static constexpr int FLAG_VISIBLE = 0x1;
        static constexpr int FLAG_CLIP_RECT = 0x2;

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
        virtual ~Actor();

        void setBackground(Drawable *d);
        Drawable* getBackground();

        void setPadding(float left, float top, float right, float bottom);
        SkRect* getPadding();
        SkRect& getPadding(SkRect& out);

        void setMargin(float left, float top, float right, float bottom);
        SkRect* getMargin();
        SkRect& getMargin(SkRect& out);

        virtual int getActorType(){
            return H7_ACTOR_TYPE;
        }
        inline bool hasActorType(int expect){
            return (getActorType() & expect) != 0;
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

        /**
         * get screen x. often set by layout
         * @return the screen x
         */
        inline float getScreenX(){
            return tmpScreenX;
        }
        /**
        * get screen y.often set by layout
        * @return the screen y
        */
        inline float getScreenY(){
            return tmpScreenY;
        }
        /**
         * do layout this actor with margin. will call layout.
         * @param ex the x
         * @param ey the y
         * @param ew the w
         * @param eh the h
         */
        virtual void doLayout(float ex, float ey, float ew, float eh){
            auto margin = getMargin();
            if(margin != nullptr){
                layout(ex + margin->left(),
                       ey + margin->top(),
                       getWidth() - margin->left() - margin->right(),
                       getHeight() - margin->top() - margin->bottom()
                );
            } else{
                layout(ex, ey, getWidth(), getHeight());
            }
        }
        bool isInLayout();
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

        /**
         * request layout and invalidate
         */
        void layoutAndInvalidate();

    protected:
        /**
       * called on layout this actor.
       * @param ex the expect x in screen which is assigned by parent
       * @param ey the expect y in screen which is assigned by parent
        * @param w the parent width
        * @param h the parent height
       */
        virtual void layout(float ex, float ey, float w, float h){
            tmpScreenX = ex + x;
            tmpScreenY = ey + y;
            applyMatrix(tmpScreenX, tmpScreenY);
        }
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

        /** Transforms the specified point in screen coordinates to the actor's local coordinate system.
 * @see Stage#screenToStageCoordinates(Vector2)
         * exclude matrix
         * */
        Vector2f &screenToLocalCoordinates(Vector2f &vec);

        // exclude matrix
        Vector2f &localToScreenCoordinates(Vector2f &vec);

        // exclude matrix
        /** Transforms the specified point in the stage's coordinates to the actor's local coordinate system. */
        Vector2f &stageToLocalCoordinates(Vector2f &vec);

        // exclude matrix
        Vector2f &localCoordinatesToStage(Vector2f &vec);

        //exclude matrix
        /** Transforms the specified point in the actor's coordinates to be in the parent's coordinates. */
        Vector2f &localToParentCoordinates(Vector2f &vec);

        //exclude matrix
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
            return (_flags & FLAG_VISIBLE) != 0;
        }
        /** If false, the actor will not be drawn and will not receive touch events. Default is true. */
        inline void setVisible(bool _visible) {
            if(_visible){
                _flags |= FLAG_VISIBLE;
            } else{
                _flags &= ~FLAG_VISIBLE;
            }
        }
        inline bool isClipRect() {
            return (_flags & FLAG_CLIP_RECT) != 0;
        }
        inline void setClipRect(bool clipRect) {
            if(clipRect){
                _flags |= FLAG_CLIP_RECT;
            } else{
                _flags &= ~FLAG_CLIP_RECT;
            }
            layoutAndInvalidate();
        }
        /** Sets the width and height. */
        inline void setSize (float width, float height) {
            if (this->width != width || this->height != height) {
                _actorListenerM.preFire();
                this->width = width;
                this->height = height;
                dispatchPropertyChanged(ActorListenerManager::TYPE_SIZE);
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
                _actorListenerM.preFire();
                this->x = x;
                dispatchPropertyChanged(ActorListenerManager::TYPE_POSITION);
            }
        }
        /** Returns the Y position of the actor's bottom edge. */
        inline float getY() {
            return y;
        }
        inline void setY(float y) {
            if (this->y != y) {
                _actorListenerM.preFire();
                this->y = y;
                dispatchPropertyChanged(ActorListenerManager::TYPE_POSITION);
            }
        }
        /** Sets the y position using the specified {@link Align alignment}. Note this may set the position to non-integer
         * coordinates. */
        inline void setY(float y, int alignment) {
            if ((alignment & Align::bottom) != 0)
                y -= height;
            else if ((alignment & Align::center) != 0)
                y -= height / 2;
            setY(y);
        }
        /** Sets the x position using the specified {@link Align alignment}. Note this may set the position to non-integer
         * coordinates. */
        inline void setX(float x, int alignment) {
            if ((alignment & Align::right) != 0)
                x -= width;
            else if ((alignment & Align::center) != 0)
                x -= width / 2;
            setX(x);
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

        float getAlpha() const;

        void setAlpha(float alpha);

        inline float getTranslateX(){
            return transX;
        }
        inline float getTranslateY(){
            return transY;
        }
        inline void setTranslateX(float x){
            if(transX != x){
                _actorListenerM.preFire();
                transX = x;
                dispatchPropertyChanged(ActorListenerManager::TYPE_TRANSLATE);
            }
        }
        inline void setTranslateY(float y){
            if(transY != y){
                _actorListenerM.preFire();
                transY = y;
                dispatchPropertyChanged(ActorListenerManager::TYPE_TRANSLATE);
            }
        }
        /** Sets the position of the actor's bottom left corner. */
        inline void setPosition (float x, float y) {
            if (this->x != x || this->y != y) {
                _actorListenerM.preFire();
                this->x = x;
                this->y = y;
                dispatchPropertyChanged(ActorListenerManager::TYPE_POSITION);
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
            setPosition(x, y);
        }
        void setBounds (float x, float y, float width, float height) {
            setPosition(x, y);
            setSize(width, height);
        }
        /** Add x and y to current position */
        inline void moveBy(float x, float y) {
            if (x != 0 || y != 0) {
                setPosition(this->x + x, this->y + y);
            }
        }
        inline float getWidth() {
            return width;
        }
        inline void setWidth(float width) {
            if (this->width != width) {
                _actorListenerM.preFire();
                this->width = width;
                dispatchPropertyChanged(ActorListenerManager::TYPE_SIZE);
            }
        }
        inline float getHeight () {
            return height;
        }
        inline void setHeight(float height) {
            if (this->height != height) {
                _actorListenerM.preFire();
                this->height = height;
                dispatchPropertyChanged(ActorListenerManager::TYPE_SIZE);
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
        /** if not set, default is center */
        inline float getOriginX () {
            return originX;
        }
        /** if not set, default is center */
        inline float getOriginY () {
            return originY;
        }
        /** Sets the origin position which is relative to the actor's bottom left corner. */
        inline void setOrigin(float originX, float originY) {
            if(this->originX != originX || this->originY != originY){
                this->originX = originX;
                this->originY = originY;
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
                _actorListenerM.preFire();
                this->scaleX = scaleX;
                this->scaleY = scaleY;
                dispatchPropertyChanged(ActorListenerManager::TYPE_SCALE);
            }
        }

        /** Adds the specified scale to the current scale. */
        inline void scaleBy (float scale) {
            scaleBy(scale, scale);
        }

        /** Adds the specified scale to the current scale. */
        inline void scaleBy (float scaleX, float scaleY) {
            if (scaleX != 0 || scaleY != 0) {
                setScale(this->scaleX + scaleX, this->scaleY + scaleY);
            }
        }
        inline float getRotation () {
            return rotation;
        }

        inline void setRotation (float degrees) {
            if (this->rotation != degrees) {
                _actorListenerM.preFire();
                this->rotation = skf_mod(degrees, 360);
                dispatchPropertyChanged(ActorListenerManager::TYPE_ROTATION);
            }
        }
        /** Adds the specified rotation to the current rotation. */
        inline void rotateBy (float amountInDegrees) {
            if (amountInDegrees != 0) {
                setRotation(rotation + amountInDegrees);
            }
        }

        inline void setColor (Color& color) {
            this->color.set(color);
        }
        inline void setColor(float r, float g, float b, float a) {
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
        inline void toBack() {
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
        void dispatchPropertyChanged(uint8_t type){
            switch (type) {
                case ActorListenerManager::TYPE_POSITION:
                    positionChanged();
                    break;
                case ActorListenerManager::TYPE_ROTATION:
                    rotationChanged();
                    break;
                case ActorListenerManager::TYPE_SCALE:
                    scaleChanged();
                    break;
                case ActorListenerManager::TYPE_SIZE:
                    sizeChanged();
                    break;
                case ActorListenerManager::TYPE_TRANSLATE:
                    translateChanged();
                    break;
                case ActorListenerManager::TYPE_ALPHA:
                    alphaChanged();
                    break;
            }
            _actorListenerM.fire(type);
        }
        /** Called when the actor's position has been changed. */
        virtual void positionChanged() {
            layoutAndInvalidate();
        }
        /** Called when the actor's size has been changed. */
        virtual void sizeChanged() {
            layoutAndInvalidate();
        }
        virtual void scaleChanged() {
            //applyMatrix();
        }
        virtual void rotationChanged() {
           // applyMatrix();
        }
        virtual void translateChanged(){
            //applyMatrix();
        }
        virtual void alphaChanged(){
            //applyMatrix();
        }
    private:
        inline void applyMatrix(float curX, float curY){
            //if not set center. default is the center if actor
            float cx, cy;
            if(originX < 0){
                cx = curX + width / 2;
            } else{
                cx = originX + curX;
            }
            if(originY < 0){
                cy = curY + height / 2;
            } else{
                cy = originY + curY;
            }
            _mat.setIdentity();
            //translate -> rotate -> scale
            if(transX > 0 || transY > 0){
                _mat.postTranslate(transX, transY);
            }
            if(rotation > 0){
                _mat.postRotate(rotation, cx, cy);
            }
            if(scaleX != 1 || scaleY != 1){
                _mat.postScale(scaleX, scaleY, cx, cy);
            }
        }
    };
}
#endif //BGFX_STUDY_ACTOR_H
