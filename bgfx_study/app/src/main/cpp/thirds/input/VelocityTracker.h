//
// Created by Administrator on 2020/11/22 0022.
//

#ifndef BGFX_STUDY_VELOCITYTRACKER_H
#define BGFX_STUDY_VELOCITYTRACKER_H

namespace h7 {
    class VelocityTracker {
    public:
        const static int sampleSize = 10;
        float lastX, lastY;
        float deltaX, deltaY;
        unsigned long long lastTime;
        int numSamples;
        float meanX[sampleSize];
        float meanY[sampleSize];
        float meanTime[sampleSize];
    public:

        void start(float x, float y, unsigned long long timeStamp);

        void update(float x, float y, unsigned long long currTime);

        float getVelocityX();

        float getVelocityY();

        float getAverage(float *values, int numSamples);

        long getAverage(long *values, int numSamples);

        float getSum(float *values, int numSamples);
    };
}
#endif //BGFX_STUDY_VELOCITYTRACKER_H
