//
// Created by Administrator on 2020/11/20 0020.
//

#ifndef BGFX_STUDY_INPUTPROCESSOR_H
#define BGFX_STUDY_INPUTPROCESSOR_H

namespace h7 {
    class InputProcessor {
    public:
        /** Called when a key was pressed
	 *
	 * @param keycode one of the constants in {@link Input.Keys}
	 * @return whether the input was processed */
        virtual bool keyDown(int keycode) = 0;

        /** Called when a key was released
         *
         * @param keycode one of the constants in {@link Input.Keys}
         * @return whether the input was processed */
        virtual bool keyUp(int keycode) = 0;

        /** Called when a key was typed
         *
         * @param character The character
         * @return whether the input was processed */
        virtual bool keyTyped(char character) = 0;

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
}

#endif //BGFX_STUDY_INPUTPROCESSOR_H
