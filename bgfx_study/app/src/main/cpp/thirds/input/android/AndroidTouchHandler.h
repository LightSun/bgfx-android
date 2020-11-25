//
// Created by Administrator on 2020/11/24 0024.
//

#ifndef BGFX_STUDY_ANDROIDTOUCHHANDLER_H
#define BGFX_STUDY_ANDROIDTOUCHHANDLER_H

#include "jni.h"

namespace h7{
    class AndroidInput;
    class MotionEventWrapper;
    class KeyEventWrapper;

    /**
     * called on android onTouch
     * @param ainput the input
     * @param event the event
     */
    void android_onTouch(AndroidInput* ainput, MotionEventWrapper* event);

    /**
     * called on android onKey
     * @param ainput the input
     * @param keyCode the key code
     * @param event the event
     * @return true if handled.
     */
    bool android_onKey(AndroidInput* ainput, int keyCode, KeyEventWrapper* event);
}

#endif //BGFX_STUDY_ANDROIDTOUCHHANDLER_H
