//
// Created by Administrator on 2020/11/27 0027.
//

#ifndef BGFX_STUDY_STAGE_H
#define BGFX_STUDY_STAGE_H

#include <lua/SkWeakRefCnt.h>
#include "../input/Input.h"
#include "utils/Array.h"
#include "math/Vector2f.h"
#include "Viewport.h"
#include "../input/InputProcessor.h"

namespace NanoCanvas{
    class Canvas;
}
struct NVGcontext;

namespace h7 {
    class Actor;

    class Action;

    class Group;

    class EventListener;

    class TouchFocus;

#define STAGE_TOUCH_NUM 20

    class Stage : public InputProcessor, public SkWeakRefCnt {
    private:
        bool actionsRequestRendering;
        sk_sp<Actor> pointerOverActors[STAGE_TOUCH_NUM];
        bool pointerTouched[STAGE_TOUCH_NUM];
        int pointerScreenX[STAGE_TOUCH_NUM];
        int pointerScreenY[STAGE_TOUCH_NUM];
        int mouseScreenX, mouseScreenY;

        Vector2f tempCoords;

        sk_sp<Actor> mouseOverActor;
        sk_sp<Actor> keyboardFocus, scrollFocus;
        sk_sp<Group> root;
        //canvas to draw
        sk_sp<NanoCanvas::Canvas> _canvas;
        sk_sp<Viewport> viewport;
        bool _isInLayout;

    public:
        Array<sk_sp<TouchFocus>> touchFocuses = Array<sk_sp<TouchFocus>>(16);

    public:
        /**
         * create stage with viewport and canvas.
         * @param vp viewport. this will auto manage ptr
         * @param canvas canvas. this will auto manage ptr
         */
        Stage(Viewport* vp, NanoCanvas::Canvas* canvas);
        Stage(Viewport* vp, NVGcontext* context);

        inline Viewport &getViewport() {
            return *viewport;
        }
        /** If true, any actions executed during a call to {@link #act()}) will result in a call to
	 * {@link Graphics#requestRendering()}. Widgets that animate or otherwise require additional rendering may check this setting
	 * before calling {@link Graphics#requestRendering()}. Default is true. */
        inline void setActionsRequestRendering(bool _actionsRequestRendering) {
            this->actionsRequestRendering = _actionsRequestRendering;
        }

        inline bool getActionsRequestRendering() {
            return actionsRequestRendering;
        }

/** Returns the {@link Actor} at the specified location in stage coordinates. Hit testing is performed in the order the actors
	 * were inserted into the stage, last inserted actors being tested first. To get stage coordinates from screen coordinates, use
	 * {@link #screenToStageCoordinates(Vector2)}.
	 * @param touchable If true, the hit detection will respect the {@link Actor#setTouchable(Touchable) touchability}.
	 * @return May be null if no actor was hit. */
        const Actor *hit(float stageX, float stageY, bool touchable);

        /** Calls the {@link Actor#act(float)} method on each actor in the stage. Typically called each frame. This method also fires
         * enter and exit events.
         * @param delta Time in seconds since the last frame. */
        void act(float delta);

        virtual void draw();
        /**
         * dispatch draw cmd.
         */
        void invalidate();
        void layout();
        bool isInLayout();

        /** Applies a touch down event to the stage and returns true if an actor in the scene {@link Event#handle() handled} the
	 * event. */
        bool touchDown(int screenX, int screenY, int pointer, int button) override ;

        /** Applies a touch up event to the stage and returns true if an actor in the scene {@link Event#handle() handled} the event.
        * Only {@link InputListener listeners} that returned true for touchDown will receive this event. */
        bool touchUp(int screenX, int screenY, int pointer, int button) override;

        /** Applies a touch moved event to the stage and returns true if an actor in the scene {@link Event#handle() handled} the
 * event. Only {@link InputListener listeners} that returned true for touchDown will receive this event. */
        bool touchDragged(int screenX, int screenY, int pointer) override;

        /** Applies a mouse moved event to the stage and returns true if an actor in the scene {@link Event#handle() handled} the
       * event. This event only occurs on the desktop. */
        bool mouseMoved(int screenX, int screenY) override;

        /** Applies a mouse scroll event to the stage and returns true if an actor in the scene {@link Event#handle() handled} the
	 * event. This event only occurs on the desktop. */
        bool scrolled(float amountX, float amountY) override;

        /** Applies a key down event to the actor that has {@link Stage#setKeyboardFocus(Actor) keyboard focus}, if any, and returns
	 * true if the event was {@link Event#handle() handled}. */
        bool keyDown(int keyCode) override;

        /** Applies a key up event to the actor that has {@link Stage#setKeyboardFocus(Actor) keyboard focus}, if any, and returns true
        * if the event was {@link Event#handle() handled}. */
        bool keyUp(int keyCode) override;

        /** Applies a key typed event to the actor that has {@link Stage#setKeyboardFocus(Actor) keyboard focus}, if any, and returns
        * true if the event was {@link Event#handle() handled}. */
        bool keyTyped(char character) override;

        /** Adds the listener to be notified for all touchDragged and touchUp events for the specified pointer and button. Touch focus
	 * is added automatically when true is returned from {@link InputListener#touchDown(InputEvent, float, float, int, int)
	 * touchDown}. The specified actors will be used as the {@link Event#getListenerActor() listener actor} and
	 * {@link Event#getTarget() target} for the touchDragged and touchUp events. */
        void addTouchFocus(const EventListener *listener, const Actor *listenerActor,
                           const Actor *target, int pointer, int button);

        /** Removes touch focus for the specified listener, pointer, and button. Note the listener will not receive a touchUp event
       * when this method is used. */
        void removeTouchFocus(const EventListener *listener, const Actor *listenerActor,
                              const Actor *target, int pointer, int button);

        /** Cancels touch focus for all listeners with the specified listener actor.
         * @see #cancelTouchFocus() */
        void cancelTouchFocus(const Actor *listenerActor);

        /** Adds an actor to the root of the stage.
	 * @see Group#addActor(Actor) */
        void addActor(Actor *actor);

        /** Adds an action to the root of the stage.
         * @see Group#addAction(Action) */
        void addAction(Action *action);

        /** Returns the root's child actors.
         * @see Group#getChildren() */
        Array<sk_sp<Actor>> &getActors();

        /** Removes the touch, keyboard, and scroll focus for the specified actor and any descendants. */
        void unfocus(Actor *actor);

        bool setScrollFocus(Actor *actor);

        bool setKeyboardFocus(Actor *actor);

        /** Gets the actor that will receive key events.
	 * @return May be null. */
        inline Actor *getKeyboardFocus() {
            return keyboardFocus.get();
        }

        inline Actor *getScrollFocus() {
            return scrollFocus.get();
        }

        /** The viewport's world width. */
        inline float getWidth() {
            return viewport->width;
        }

        /** The viewport's world height. */
        inline float getHeight() {
            return viewport->height;
        }

        /** Returns the root group which holds all actors in the stage. */
        inline Group *getRoot() {
            return root.get();
        }

        /** Replaces the root group. This can be useful, for example, to subclass the root group to be notified by
         * {@link Group#childrenChanged()}. */
        void setRoot(Group *root);

        Vector2f &screenToStageCoordinates(Vector2f &f);

        Vector2f &stageToScreenCoordinates(Vector2f &f);

    private:
        inline sk_sp<Actor> fireEnterAndExit(sk_sp<Actor>& overLast, int screenX, int screenY, int pointer);
    };

    class TouchFocus : public SkRefCnt {
    public:
        sk_sp<EventListener> listener;
        sk_sp<Actor> listenerActor, target;
        int pointer, button;

        void reset() {
            listenerActor = nullptr;
            listener = nullptr;
            target = nullptr;
        }
    };
}

#endif //BGFX_STUDY_STAGE_H
