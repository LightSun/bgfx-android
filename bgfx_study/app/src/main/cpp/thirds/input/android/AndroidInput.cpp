//
// Created by Administrator on 2020/11/24 0024.
//

#include <stdlib.h>
#include "AndroidInput.h"
#include "../../../core/log.h"

namespace h7{
    void AndroidInput::release() {
        if(pressure) free(pressure);
        if(realId) free(realId);
        if(touchX) free(touchX);
        if(touchY) free(touchY);
        if(deltaX) free(deltaX);
        if(deltaY) free(deltaY);
        if(touched) free(touched);
        if(button) free(button);
    }
    void AndroidInput::ensureSize(int expect) {
        int oldLen = len_realId;
        int* realId = (int*)malloc(sizeof(int) * expect);
        int* touchX = (int*)malloc(sizeof(int) * expect);
        int* touchY = (int*)malloc(sizeof(int) * expect);
        int* deltaX = (int*)malloc(sizeof(int) * expect);
        int* deltaY = (int*)malloc(sizeof(int) * expect);
        int* button = (int*)malloc(sizeof(int) * expect);

        float* pressure = (float*)malloc(sizeof(float) * expect);
        bool* touched = (bool*)malloc(sizeof(bool) * expect);
#define RESET_FIELD(x, unitType)\
if(this->x){\
    memcpy(x, this->x, oldLen * sizeof(unitType));\
    free(this->x);\
    this->x = x;\
}
        //copy value and release old
        RESET_FIELD(realId, int)
        RESET_FIELD(touchX, int)
        RESET_FIELD(touchY, int)
        RESET_FIELD(deltaX, int)
        RESET_FIELD(deltaY, int)
        RESET_FIELD(button, int)
        RESET_FIELD(pressure, float)
        RESET_FIELD(touched, bool)

        len_realId = expect;
    }
    AndroidInput::AndroidInput() {
        ensureSize(NUM_TOUCHES);
        for (int i = 0; i < len_realId; i++)
            realId[i] = -1;
    }
    AndroidInput::~AndroidInput() {
        release();
    }
    int AndroidInput::getFreePointerIndex() {
        int len = len_realId;
        for (int i = 0; i < len; i++) {
            if (realId[i] == -1) return i;
        }
        ensureSize(len + 2);
        /*pressure = resize(pressure);
        realId = resize(realId);
        touchX = resize(touchX);
        touchY = resize(touchY);
        deltaX = resize(deltaX);
        deltaY = resize(deltaY);
        touched = resize(touched);
        button = resize(button);*/

        return len;
    }
    int AndroidInput::lookUpPointerIndex(int pointerId) {
        for (int i = 0; i < NUM_TOUCHES; i++) {
            if (realId[i] == pointerId) return i;
        }
        LOGW("lookUpPointerIndex: can't find pointer index for pointerId = %d", pointerId);
        return -1;
    }

}