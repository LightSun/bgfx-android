//
// Created by Administrator on 2020/11/29 0029.
//

#ifndef BGFX_STUDY_INPUTEVENT_H
#define BGFX_STUDY_INPUTEVENT_H

#include <limits.h>
#include <lua/SkRefCnt.h>
#include "Event.h"
#include "../../core/math/Vector2f.h"

namespace h7 {
    class Actor;

    class InputEvent : public Event {
    public:
        float stageX, stageY, scrollAmountX, scrollAmountY;
        int pointer, button, keyCode;
        char character;
        sk_sp <Actor> relatedActor;
        bool touchFocus = true;

        /** Types of low-level input events supported by scene2d. */
        enum Type {
/** A new touch for a pointer on the stage was detected */
            touchDown = 0,
            /** A pointer has stopped touching the stage. */
            touchUp = 1,
            /** A pointer that is touching the stage has moved. */
            touchDragged = 2,
            /** The mouse pointer has moved (without a mouse button being active). */
            mouseMoved = 3,
            /** The mouse pointer or an active touch have entered (i.e., {@link Actor#hit(float, float, boolean) hit}) an actor. */
            enter = 4,
            /** The mouse pointer or an active touch have exited an actor. */
            exit = 5,
            /** The mouse scroll wheel has changed. */
            scrolled = 6,
            /** A keyboard key has been pressed. */
            keyDown = 7,
            /** A keyboard key has been released. */
            keyUp = 8,
            /** A keyboard key has been pressed and released. */
            keyTyped = 9
        };
        Type type;

        void reset() {
            Event::reset();
            relatedActor = nullptr;
            button = -1;
        }

        /** The stage x coordinate where the event occurred. Valid for: touchDown, touchDragged, touchUp, mouseMoved, enter, and
         * exit. */
        inline float getStageX() {
            return stageX;
        }

        inline void setStageX(float _stageX) {
            this->stageX = _stageX;
        }

        /** The stage x coordinate where the event occurred. Valid for: touchDown, touchDragged, touchUp, mouseMoved, enter, and
         * exit. */
        inline float getStageY() {
            return stageY;
        }

        inline void setStageY(float _stageY) {
            this->stageY = _stageY;
        }

        /** The type of input event. */
        inline Type getType() {
            return type;
        }

        inline void setType(Type _type) {
            this->type = _type;
        }

        /** The pointer index for the event. The first touch is index 0, second touch is index 1, etc. Always -1 on desktop. Valid for:
         * touchDown, touchDragged, touchUp, enter, and exit. */
        inline int getPointer() {
            return pointer;
        }

        inline
        void setPointer(int _pointer) {
            this->pointer = _pointer;
        }

        /** The index for the mouse button pressed. Always 0 on Android. Valid for: touchDown and touchUp.
         * @see Buttons */

        inline int getButton() {
            return button;
        }
        inline void setButton(int _button) {
            this->button = _button;
        }

        /** The key code of the key that was pressed. Valid for: keyDown and keyUp. */

        inline int getKeyCode() {
            return keyCode;
        }

        inline void setKeyCode(int _keyCode) {
            this->keyCode = _keyCode;
        }

        /** The character for the key that was type. Valid for: keyTyped. */
        inline

        char getCharacter() {
            return character;
        }

        inline void setCharacter(char _character) {
            this->character = _character;
        }

        /** The amount the mouse was scrolled horizontally. Valid for: scrolled. */
        inline

        float getScrollAmountX() {
            return scrollAmountX;
        }
        /** The amount the mouse was scrolled vertically. Valid for: scrolled. */
        inline

        float getScrollAmountY() {
            return scrollAmountY;
        }

        inline void setScrollAmountX(float _scrollAmount) {
            this->scrollAmountX = _scrollAmount;
        }

        inline
        void setScrollAmountY(float _scrollAmount) {
            this->scrollAmountY = _scrollAmount;
        }

        /** The actor related to the event. Valid for: enter and exit. For enter, this is the actor being exited, or null. For exit,
         * this is the actor being entered, or null. */
        inline sk_sp<Actor> getRelatedActor() {
            return relatedActor;
        }

        /** @param relatedActor May be null. */
        inline
        void setRelatedActor(sk_sp<Actor> _relatedActor) {
            this->relatedActor = _relatedActor;
        }

        /** Sets actorCoords to this event's coordinates relative to the specified actor.
         * @param actorCoords Output for resulting coordinates. */
        Vector2f& toCoordinates(sk_sp<Actor> actor, Vector2f actorCoords);

        /** Returns true if this event is a touchUp triggered by {@link Stage#cancelTouchFocus()}. */
        inline bool isTouchFocusCancel() {
            return stageX == INT_MIN || stageY == INT_MIN;
        }

        /** If false, {@link InputListener#handle(Event)} will not add the listener to the stage's touch focus when a touch down event
         * is handled. Default is true. */
        inline
        bool getTouchFocus() {
            return touchFocus;
        }

        inline
        void setTouchFocus(bool _touchFocus) {
            this->touchFocus = _touchFocus;
        }

        inline
        const char* toString() {
#define TO_STR_TYPE(n) \
  case n:\
       return #n;
            switch (type){
                TO_STR_TYPE(touchDown)
                TO_STR_TYPE(touchUp)
                TO_STR_TYPE(touchDragged)
                TO_STR_TYPE(mouseMoved)
                TO_STR_TYPE(enter)
                TO_STR_TYPE(exit)
                TO_STR_TYPE(scrolled)
                TO_STR_TYPE(keyDown)
                TO_STR_TYPE(keyUp)
                TO_STR_TYPE(keyTyped)
            }
            return "null";
        }
    };
}

#endif //BGFX_STUDY_INPUTEVENT_H
