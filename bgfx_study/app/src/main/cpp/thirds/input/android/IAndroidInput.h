//
// Created by Administrator on 2020/11/25 0025.
//

#ifndef BGFX_STUDY_IANDROIDINPUT_H
#define BGFX_STUDY_IANDROIDINPUT_H

#include "RefObject.h"
#include "utils/Array.h"


namespace h7{
    class KeyEventWrapper;
    class MotionEventWrapper;

    class OnKeyListener: public RefObject{
    public:
        virtual bool onKey(int keyCode, KeyEventWrapper* event) = 0;

        class Iterator: public ArrayIterator<OnKeyListener*>{
        public:
            bool iterate(Array<OnKeyListener*>* arr, int index, OnKeyListener*& ele){
                ele->unRefAndDestroy();
                return false;
            }
        };
    };
    class OnGenericMotionListener: public RefObject{
    public:
        virtual bool onGenericMotion(MotionEventWrapper* wrapper);

        class Iterator: public ArrayIterator<OnGenericMotionListener*>{
        public:
            bool iterate(Array<OnGenericMotionListener*>* arr, int index, OnGenericMotionListener*& ele){
                ele->unRefAndDestroy();
                return false;
            }
        };
    };
    namespace InputDevice{
        static const int SOURCE_CLASS_POINTER = 2;
    }

    class IAndroidInput{
    public:
        /* Called when the {@link App} is about to pause */
       // virtual void onPause() = 0;

        /* Called when the {@link App} is about to resume */
       // virtual void onResume() = 0;

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
