//
// Created by Administrator on 2020/11/25 0025.
//

#ifndef BGFX_STUDY_IANDROIDINPUT_H
#define BGFX_STUDY_IANDROIDINPUT_H

namespace h7{
    class KeyEventWrapper;
    class MotionEventWrapper;

    class OnKeyListener {
    public:
        virtual bool onKey(int keyCode, KeyEventWrapper* event) = 0;
    };
    class OnGenericMotionListener{
    public:
    };

    class IAndroidInput{
    public:
        /** Called when the {@link App} is about to pause */
        virtual void onPause() = 0;

        /** Called when the {@link App} is about to resume */
        virtual void onResume() = 0;

        /** Add a custom {@link OnKeyListener} */
        virtual void addKeyListener(OnKeyListener* listener) = 0;

        /** Add a custom {@link OnGenericMotionListener} */
        virtual void addGenericMotionListener (OnGenericMotionListener* listener) = 0;

        /** Process all events (touch, key) that have been registered by the listeners so that event processing is synchronous. */
        virtual void processEvents() = 0;

        /** Shows/Hides soft screen keyboard */
        virtual void setKeyboardAvailable(bool available) = 0;
    };
}

#endif //BGFX_STUDY_IANDROIDINPUT_H
