//
// Created by Administrator on 2020/12/30 0030.
//

#include "Interpolation.h"

namespace h7{
    namespace Interpolations{
        static Interpolation *_linear_ins = nullptr;

        Interpolation *getlinear() {
            if (_linear_ins == nullptr) { _linear_ins = new Interpolation(); }
            return _linear_ins;
        }

        static smooth *_smooth_ins = nullptr;

        Interpolation *getsmooth() {
            if (_smooth_ins == nullptr) { _smooth_ins = new smooth(); }
            return _smooth_ins;
        }

        static smooth2 *_smooth2_ins = nullptr;

        Interpolation *getsmooth2() {
            if (_smooth2_ins == nullptr) { _smooth2_ins = new smooth2(); }
            return _smooth2_ins;
        }
        static fade *_fade_ins = nullptr;

        Interpolation *getfade() {
            if (_fade_ins == nullptr) { _fade_ins = new fade(); }
            return _fade_ins;
        }

        static pow3InInverse *_pow3InInverse_ins = nullptr;

        Interpolation *getpow3InInverse() {
            if (_pow3InInverse_ins == nullptr) { _pow3InInverse_ins = new pow3InInverse(); }
            return _pow3InInverse_ins;
        }

        static pow3OutInverse *_pow3OutInverse_ins = nullptr;

        Interpolation *getpow3OutInverse() {
            if (_pow3OutInverse_ins == nullptr) { _pow3OutInverse_ins = new pow3OutInverse(); }
            return _pow3OutInverse_ins;
        }

        static sine *_sine_ins = nullptr;

        Interpolation *getsine() {
            if (_sine_ins == nullptr) { _sine_ins = new sine(); }
            return _sine_ins;
        }
        static sineIn *_sineIn_ins = nullptr;

        Interpolation *getsineIn() {
            if (_sineIn_ins == nullptr) { _sineIn_ins = new sineIn(); }
            return _sineIn_ins;
        }
        static sineOut *_sineOut_ins = nullptr;

        Interpolation *getsineOut() {
            if (_sineOut_ins == nullptr) { _sineOut_ins = new sineOut(); }
            return _sineOut_ins;
        }
        static circle *_circle_ins = nullptr;

        Interpolation *getcircle() {
            if (_circle_ins == nullptr) { _circle_ins = new circle(); }
            return _circle_ins;
        }
        static circleIn *_circleIn_ins = nullptr;

        Interpolation *getcircleIn() {
            if (_circleIn_ins == nullptr) { _circleIn_ins = new circleIn(); }
            return _circleIn_ins;
        }
        static circleOut *_circleOut_ins = nullptr;

        Interpolation *getcircleOut() {
            if (_circleOut_ins == nullptr) { _circleOut_ins = new circleOut(); }
            return _circleOut_ins;
        }
        static pow2InInverse *_pow2InInverse_ins = nullptr;

        Interpolation *getpow2InInverse() {
            if (_pow2InInverse_ins == nullptr) { _pow2InInverse_ins = new pow2InInverse(); }
            return _pow2InInverse_ins;
        }
        static pow2OutInverse *_pow2OutInverse_ins = nullptr;

        Interpolation *getpow2OutInverse() {
            if (_pow2OutInverse_ins == nullptr) { _pow2OutInverse_ins = new pow2OutInverse(); }
            return _pow2OutInverse_ins;
        }
        static Elastic *elastic = nullptr;

        Interpolation *getelastic() {
            if (elastic == nullptr) {
                elastic = new Elastic(2.0f, 10.0f, 7, 1.0f);
            }
            return elastic;
        }
        static ElasticIn *elasticIn = nullptr;

        Interpolation *getelasticIn() {
            if (elasticIn == nullptr) {
                elasticIn = new ElasticIn(2.0f, 10.0f, 6, 1.0f);
            }
            return elasticIn;
        }
        static ElasticOut *elasticOut = nullptr;

        Interpolation *getelasticOut() {
            if (elasticOut == nullptr) {
                elasticOut = new ElasticOut(2.0f, 10.0f, 7, 1.0f);
            }
            return elasticOut;
        }
        static Swing *swing = nullptr;

        Interpolation *getswing() {
            if (swing == nullptr) { swing = new Swing(1.5f); }
            return swing;
        }

        //DEF_GET_FIELD(Swing, swing, new Swing(1.5f))
        static SwingIn *swingIn = nullptr;

        Interpolation *getswingIn() {
            if (swingIn == nullptr) { swingIn = new SwingIn(2.0f); }
            return swingIn;
        }

        //DEF_GET_FIELD(SwingIn, swingIn, new SwingIn(2.0f))
        static SwingOut *swingOut = nullptr;

        Interpolation *getswingOut() {
            if (swingOut == nullptr) { swingOut = new SwingOut(2.0f); }
            return swingOut;
        }

        //DEF_GET_FIELD(SwingOut, swingOut, new SwingOut(2.0f))
        static Bounce *bounce = nullptr;

        Interpolation *getbounce() {
            if (bounce == nullptr) { bounce = new Bounce(4); }
            return bounce;
        }

        //DEF_GET_FIELD(Bounce, bounce, new Bounce(4))
        static BounceIn *bounceIn = nullptr;

        Interpolation *getbounceIn() {
            if (bounceIn == nullptr) { bounceIn = new BounceIn(4); }
            return bounceIn;
        }

        // DEF_GET_FIELD(BounceIn, bounceIn, new BounceIn(4))
        static BounceOut *bounceOut = nullptr;

        Interpolation *getbounceOut() {
            if (bounceOut == nullptr) { bounceOut = new BounceOut(4); }
            return bounceOut;
        }

        //DEF_GET_FIELD(BounceOut, bounceOut, new BounceOut(4))
        static Exp *exp10 = nullptr;

        Interpolation *getexp10() {
            if (exp10 == nullptr) { exp10 = new Exp(2, 10); }
            return exp10;
        }

        //DEF_GET_FIELD(Exp, exp10, new Exp(2, 10))
        static ExpIn *exp10In = nullptr;

        Interpolation *getexp10In() {
            if (exp10In == nullptr) { exp10In = new ExpIn(2, 10); }
            return exp10In;
        }

        //DEF_GET_FIELD(ExpIn, exp10In, new ExpIn(2, 10))
        static ExpOut *exp10Out = nullptr;

        Interpolation *getexp10Out() {
            if (exp10Out == nullptr) { exp10Out = new ExpOut(2, 10); }
            return exp10Out;
        }

        // DEF_GET_FIELD(ExpOut, exp10Out, new ExpOut(2, 10))
        static Exp *exp5 = nullptr;

        Interpolation *getexp5() {
            if (exp5 == nullptr) { exp5 = new Exp(2, 5); }
            return exp5;
        }

        //DEF_GET_FIELD(Exp, exp5, new Exp(2, 5))
        static ExpIn *exp5In = nullptr;

        Interpolation *getexp5In() {
            if (exp5In == nullptr) { exp5In = new ExpIn(2, 5); }
            return exp5In;
        }

        //DEF_GET_FIELD(ExpIn, exp5In, new ExpIn(2, 5))
        static ExpOut *exp5Out = nullptr;

        Interpolation *getexp5Out() {
            if (exp5Out == nullptr) { exp5Out = new ExpOut(2, 5); }
            return exp5Out;
        }

        //DEF_GET_FIELD(ExpOut, exp5Out, new ExpOut(2, 5))
        static Power *pow2 = nullptr;

        Interpolation *getpow2() {
            if (pow2 == nullptr) { pow2 = new Power(2); }
            return pow2;
        }

        //DEF_GET_FIELD(Power, pow2, new Power(2))
        static PowerIn *pow2In = nullptr;

        Interpolation *getpow2In() {
            if (pow2In == nullptr) { pow2In = new PowerIn(2); }
            return pow2In;
        }

        //DEF_GET_FIELD(PowerIn, pow2In, new PowerIn(2))
        static PowerOut *pow2Out = nullptr;

        Interpolation *getpow2Out() {
            if (pow2Out == nullptr) { pow2Out = new PowerOut(2); }
            return pow2Out;
        }

        //DEF_GET_FIELD(PowerOut, pow2Out, new PowerOut(2))
        static Power *pow3 = nullptr;

        Interpolation *getpow3() {
            if (pow3 == nullptr) { pow3 = new Power(3); }
            return pow3;
        }

        //DEF_GET_FIELD(Power, pow3, new Power(3))
        static PowerIn *pow3In = nullptr;

        Interpolation *getpow3In() {
            if (pow3In == nullptr) { pow3In = new PowerIn(3); }
            return pow3In;
        }

        //DEF_GET_FIELD(PowerIn, pow3In, new PowerIn(3))
        static PowerOut *pow3Out = nullptr;

        Interpolation *getpow3Out() {
            if (pow3Out == nullptr) { pow3Out = new PowerOut(3); }
            return pow3Out;
        }

        //DEF_GET_FIELD(PowerOut, pow3Out, new PowerOut(3))
        static Power *pow4 = nullptr;

        Interpolation *getpow4() {
            if (pow4 == nullptr) { pow4 = new Power(4); }
            return pow4;
        }

        //DEF_GET_FIELD(Power, pow4, new Power(4))
        static PowerIn *pow4In = nullptr;

        Interpolation *getpow4In() {
            if (pow4In == nullptr) { pow4In = new PowerIn(4); }
            return pow4In;
        }

        //DEF_GET_FIELD(PowerIn, pow4In, new PowerIn(4))
        static PowerOut *pow4Out = nullptr;

        Interpolation *getpow4Out() {
            if (pow4Out == nullptr) { pow4Out = new PowerOut(4); }
            return pow4Out;
        }

        //DEF_GET_FIELD(PowerOut, pow4Out, new PowerOut(4))
        static Power *pow5 = nullptr;

        Interpolation *getpow5() {
            if (pow5 == nullptr) { pow5 = new Power(5); }
            return pow5;
        }

        //DEF_GET_FIELD(Power, pow5, new Power(5))
        static PowerIn *pow5In = nullptr;

        Interpolation *getpow5In() {
            if (pow5In == nullptr) { pow5In = new PowerIn(5); }
            return pow5In;
        }

        //DEF_GET_FIELD(PowerIn, pow5In, new PowerIn(5))
        static PowerOut *pow5Out = nullptr;

        Interpolation *getpow5Out() {
            if (pow5Out == nullptr) { pow5Out = new PowerOut(5); }
            return pow5Out;
        }

        class ViscousFluidInterpolation: public Interpolation {
        private:
            /** Controls the viscous fluid effect (how much of it). */
            static constexpr float VISCOUS_FLUID_SCALE = 8.0f;
            static float VISCOUS_FLUID_NORMALIZE;
            static float VISCOUS_FLUID_OFFSET;

            static inline float viscousFluid(float x) {
                x *= VISCOUS_FLUID_SCALE;
                if (x < 1.0f) {
                    x -= (1.0f - bx::exp(-x));
                } else {
                    float start = 0.36787944117f;   // 1/e == exp(-1)
                    x = 1.0f - bx::exp(1.0f - x);
                    x = start + x * (1.0f - start);
                }
                return x;
            }
        public:
            static inline void init(){
                // must be set to 1.0 (used in viscousFluid())
                VISCOUS_FLUID_NORMALIZE = 1.0f / viscousFluid(1.0f);
                // account for very small floating-point error
                VISCOUS_FLUID_OFFSET = 1.0f - VISCOUS_FLUID_NORMALIZE * viscousFluid(1.0f);
            }
            virtual float apply(float input) {
                const float interpolated = VISCOUS_FLUID_NORMALIZE * viscousFluid(input);
                if (interpolated > 0) {
                    return interpolated + VISCOUS_FLUID_OFFSET;
                }
                return interpolated;
            }
        };

        static Interpolation* _ViscousFluid = nullptr;
        Interpolation *getViscousFluid(){
            if(_ViscousFluid == nullptr){
                _ViscousFluid = new ViscousFluidInterpolation();
                ViscousFluidInterpolation::init();
            }
            return _ViscousFluid;
        }

    }
}