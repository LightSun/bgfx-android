//
// Created by Administrator on 2020/11/22 0022.
//

#include "VelocityTracker.h"
#include <math.h>
using namespace h7;

 void VelocityTracker::start (float x, float y, unsigned long long timeStamp) {
    lastX = x;
    lastY = y;
    deltaX = 0;
    deltaY = 0;
    numSamples = 0;
    for (int i = 0; i < sampleSize; i++) {
        meanX[i] = 0;
        meanY[i] = 0;
        meanTime[i] = 0;
    }
    lastTime = timeStamp;
}

 void VelocityTracker::update (float x, float y, unsigned long long currTime) {
    deltaX = x - lastX;
    deltaY = y - lastY;
    lastX = x;
    lastY = y;
    long deltaTime = currTime - lastTime;
    lastTime = currTime;
    int index = numSamples % sampleSize;
    meanX[index] = deltaX;
    meanY[index] = deltaY;
    meanTime[index] = deltaTime;
    numSamples++;
}

 float VelocityTracker::getVelocityX () {
    float meanX = getAverage(this.meanX, numSamples);
    float meanTime = getAverage(this.meanTime, numSamples) / 1000000000.0f;
    if (meanTime == 0) return 0;
    return meanX / meanTime;
}

 float VelocityTracker::getVelocityY () {
    float meanY = getAverage(this.meanY, numSamples);
    float meanTime = getAverage(this.meanTime, numSamples) / 1000000000.0f;
    if (meanTime == 0) return 0;
    return meanY / meanTime;
}

 float VelocityTracker::getAverage (float* values, int numSamples) {
    numSamples = fminf(sampleSize, numSamples);
    float sum = 0;
    for (int i = 0; i < numSamples; i++) {
        sum += values[i];
    }
    return sum / numSamples;
}

 long VelocityTracker::getAverage (long* values, int numSamples) {
    numSamples = fminf(sampleSize, numSamples);
    long sum = 0;
    for (int i = 0; i < numSamples; i++) {
        sum += values[i];
    }
    if (numSamples == 0) return 0;
    return sum / numSamples;
}

 float VelocityTracker::getSum (float* values, int numSamples) {
    numSamples = fminf(sampleSize, numSamples);
    float sum = 0;
    for (int i = 0; i < numSamples; i++) {
        sum += values[i];
    }
    if (numSamples == 0) return 0;
    return sum;
}