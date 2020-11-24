//
// Created by Administrator on 2020/11/24 0024.
//

#ifndef BGFX_STUDY_ANDROIDTOUCHHANDLER_H
#define BGFX_STUDY_ANDROIDTOUCHHANDLER_H
namespace h7{
    class AndroidInput;
    class MotionEventWrapper;

    void android_onTouch(AndroidInput* ainput, MotionEventWrapper* event);
}

#endif //BGFX_STUDY_ANDROIDTOUCHHANDLER_H
