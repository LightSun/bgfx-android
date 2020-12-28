//
// Created by Administrator on 2020/12/29 0029.
//

#ifndef BGFX_STUDY_VIEWCONFIGURATION_H
#define BGFX_STUDY_VIEWCONFIGURATION_H

namespace h7{

    class ViewConfiguration{

    public:
        /** Earth's gravity in SI units (m/s^2) */
        constexpr static float GRAVITY_EARTH   = 9.80665f;

        static float getScrollFriction(){
            return SCROLL_FRICTION;
        }
    private:
        /**
        * The coefficient of friction applied to flings/scrolls.
        */
        static constexpr float SCROLL_FRICTION = 0.015f;

    };
}

#endif //BGFX_STUDY_VIEWCONFIGURATION_H
