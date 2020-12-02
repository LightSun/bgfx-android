//
// Created by Administrator on 2020/11/20 0020.
//

#ifndef BGFX_STUDY_INPUTPROCESSOR_H
#define BGFX_STUDY_INPUTPROCESSOR_H

#include "../../core/math/Vector2f.h"

namespace h7 {
    class InputProcessor{
    public:
        /** Called when a key was pressed
	 *
	 * @param keycode one of the constants in {@link Input.Keys}
	 * @return whether the input was processed */
        virtual bool keyDown(int keycode){ return false;}

        /** Called when a key was released
         *
         * @param keycode one of the constants in {@link Input.Keys}
         * @return whether the input was processed */
        virtual bool keyUp(int keycode){ return false;}

        /** Called when a key was typed
         *
         * @param character The character
         * @return whether the input was processed */
        virtual bool keyTyped(char character){ return false;}

        /** Called when the screen was touched or a mouse button was pressed. The button parameter will be {@link Buttons#LEFT} on iOS.
         * @param screenX The x coordinate, origin is in the upper left corner
         * @param screenY The y coordinate, origin is in the upper left corner
         * @param pointer the pointer for the event.
         * @param button the button
         * @return whether the input was processed */
        virtual bool touchDown(int screenX, int screenY, int pointer, int button) = 0;

        /** Called when a finger was lifted or a mouse button was released. The button parameter will be {@link Buttons#LEFT} on iOS.
         * @param pointer the pointer for the event.
         * @param button the button
         * @return whether the input was processed */
        virtual bool touchUp(int screenX, int screenY, int pointer, int button) = 0;

        /** Called when a finger or the mouse was dragged.
         * @param pointer the pointer for the event.
         * @return whether the input was processed */
        virtual bool touchDragged(int screenX, int screenY, int pointer) = 0;

        /** Called when the mouse was moved without any buttons being pressed. Will not be called on iOS.
         * @return whether the input was processed */
        virtual bool mouseMoved(int screenX, int screenY) = 0;

        /** Called when the mouse wheel was scrolled. Will not be called on iOS.
         * @param amountX the horizontal scroll amount, negative or positive depending on the direction the wheel was scrolled.
         * @param amountY the vertical scroll amount, negative or positive depending on the direction the wheel was scrolled.
         * @return whether the input was processed. */
        virtual bool scrolled(float amountX, float amountY) = 0;
    };

    class GestureListener{
    public:
        /** @see InputProcessor#touchDown(int, int, int, int) */
        virtual bool touchDown(float x, float y, int pointer, int button) { return false; }

        /** Called when a tap occured. A tap happens if a touch went down on the screen and was lifted again without moving outside
         * of the tap square. The tap square is a rectangular area around the initial touch position as specified on construction
         * time of the {@link GestureDetector}.
         * @param count the number of taps. */
        virtual bool tap(float x, float y, int count, int button) { return false; }

        virtual bool longPress(float x, float y) { return false; }

        /** Called when the user dragged a finger over the screen and lifted it. Reports the last known velocity of the finger in
         * pixels per second.
         * @param velocityX velocity on x in seconds
         * @param velocityY velocity on y in seconds */
        virtual bool fling(float velocityX, float velocityY, int button) { return false; }

        /** Called when the user drags a finger over the screen.
         * @param deltaX the difference in pixels to the last drag event on x.
         * @param deltaY the difference in pixels to the last drag event on y. */
        virtual bool pan(float x, float y, float deltaX, float deltaY) { return false; }

        /** Called when no longer panning. */
        virtual bool panStop(float x, float y, int pointer, int button) { return false; }

        /** Called when the user performs a pinch zoom gesture. The original distance is the distance in pixels when the gesture
         * started.
         * @param initialDistance distance between fingers when the gesture started.
         * @param distance current distance between fingers. */
        virtual bool zoom(float initialDistance, float distance) { return false; }

        /** Called when a user performs a pinch zoom gesture. Reports the initial positions of the two involved fingers and their
         * current positions.
         * @param initialPointer1
         * @param initialPointer2
         * @param pointer1
         * @param pointer2 */
        virtual bool pinch(Vector2f &initialPointer1, Vector2f &initialPointer2, Vector2f &pointer1,
                           Vector2f &pointer2) { return false; }

        /** Called when no longer pinching. */
        virtual void pinchStop() {}
    };
}

#endif //BGFX_STUDY_INPUTPROCESSOR_H
