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
        release();
        realId = (int*)malloc(sizeof(int) * expect);
        touchX = (int*)malloc(sizeof(int) * expect);
        touchY = (int*)malloc(sizeof(int) * expect);
        deltaX = (int*)malloc(sizeof(int) * expect);
        deltaY = (int*)malloc(sizeof(int) * expect);
        button = (int*)malloc(sizeof(int) * expect);

        pressure = (float*)malloc(sizeof(float) * expect);
        touched = (bool*)malloc(sizeof(bool) * expect);
        len_realId = expect;
    }
    AndroidInput::AndroidInput() {
        ensureSize(NUM_TOUCHES);
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