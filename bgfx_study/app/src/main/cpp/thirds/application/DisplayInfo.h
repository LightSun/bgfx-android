//
// Created by Administrator on 2020/12/28 0028.
//

#ifndef BGFX_STUDY_DISPLAYINFO_H
#define BGFX_STUDY_DISPLAYINFO_H

namespace h7{
    struct DisplayInfo{
    public:
        int width;
        int height;
        float density;
        float ppi;  //for android:  float ppi = context.getResources().getDisplayMetrics().density * 160.0f;
    };
}

#endif //BGFX_STUDY_DISPLAYINFO_H
