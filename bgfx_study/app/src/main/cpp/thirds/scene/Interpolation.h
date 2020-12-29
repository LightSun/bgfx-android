//
// Created by Administrator on 2020/12/3 0003.
//

#ifndef BGFX_STUDY_INTERPOLATION_H
#define BGFX_STUDY_INTERPOLATION_H

#include <string.h>
#include <stdlib.h>
#include <cmath> //cbrt need c++ 11
#include "../../include/bx/math.h"

/*#define FLOAT_ROUNDING_ERROR 0.000001f

#define DEF_INTERPOLATION(type, x, result)\
    class type: public Interpolation {\
    public:\
        virtual float apply(float a) {\
            x;\
            return result;\
        }\
    }; \
    static type* _##type##_ins = nullptr;\
    Interpolation* get##type(){ \
            if(_##type##_ins == nullptr){\
                _##type##_ins = new type();\
            }\
            return _##type##_ins;\
        }

#define DEF_INTERPOLATION2(type, result) DEF_INTERPOLATION(type, , result)
#define  DEF_GET_FIELD(type, fn, newExpr) \
    static type* fn;\
    Interpolation* get##fn(){\
        if(fn == nullptr){\
            fn = newExpr;\
        }\
        return fn;\
    }

#define GET_FIELD(type, old_name, fn)\
    Interpolation* get##fn(){\
        return get##old_name();\
    }*/

namespace h7 {
    /** Takes a linear value in the range of 0-1 and outputs a (usually) non-linear, interpolated value.
     * */
    class Interpolation {
    public:
        //from 0-1 => 0-1
        virtual float apply(float a) {
            return a;
        }

        /** normalize the apply result to a range. [start, end] */
        inline float apply(float start, float end, float a) {
            return start + (end - start) * apply(a);
        }

    };
    namespace Interpolations {
        class Power : public Interpolation {
        protected:
            int power;
        public:
            Power(int power) : power(power) {}

            float apply(float a) {
                if (a <= 0.5f) return bx::pow(a * 2, power) / 2;
                return bx::pow((a - 1) * 2, power) / (power % 2 == 0 ? -2 : 2) + 1;
            }
        };

        class PowerIn : public Power {
        public:
            PowerIn(int power) : Power(power) {}

            float apply(float a) {
                return bx::pow(a, power);
            }
        };

        class PowerOut : public Power {
        public:
            PowerOut(int power) : Power(power) {}

            float apply(float a) {
                return bx::pow(a - 1, power) * (power % 2 == 0 ? -1 : 1) + 1;
            }
        };

        class Exp : public Interpolation {
        protected:
            float value, power, min, scale;
        public:
            Exp(float value, float power) {
                this->value = value;
                this->power = power;
                min = bx::pow(value, -power);
                scale = 1 / (1 - min);
            }

            float apply(float a) {
                if (a <= 0.5f) return (bx::pow(value, power * (a * 2 - 1)) - min) * scale / 2;
                return (2 - (bx::pow(value, -power * (a * 2 - 1)) - min) * scale) / 2;
            }
        };

        class ExpIn : public Exp {
        public:
            ExpIn(float value, float power) : Exp(value, power) {}

            float apply(float a) {
                return (bx::pow(value, power * (a - 1)) - min) * scale;
            }
        };

        class ExpOut : public Exp {
        public:
            ExpOut(float value, float power) : Exp(value, power) {}

            float apply(float a) {
                return 1 - (bx::pow(value, -power * a) - min) * scale;
            }
        };

        class Elastic : public Interpolation {
        protected:
            float value, power, scale, bounces;

        public:
            Elastic(float value, float power, int bounces, float scale) {
                this->value = value;
                this->power = power;
                this->scale = scale;
                this->bounces = bounces * bx::kPi * (bounces % 2 == 0 ? 1 : -1);
            }

            float apply(float a) {
                if (a <= 0.5f) {
                    a *= 2;
                    return bx::pow(value, power * (a - 1)) * bx::sin(a * bounces) * scale / 2;
                }
                a = 1 - a;
                a *= 2;
                return 1 - bx::pow(value, power * (a - 1)) * bx::sin((a) * bounces) * scale / 2;
            }
        };

        class ElasticIn : public Elastic {
        public:
            ElasticIn(float value, float power, int bounces, float scale) : Elastic(value, power,
                                                                                    bounces,
                                                                                    scale) {
            }

            float apply(float a) {
                if (a >= 0.99) return 1;
                return bx::pow(value, power * (a - 1)) * bx::sin(a * bounces) * scale;
            }
        };

        class ElasticOut : public Elastic {
        public:
            ElasticOut(float value, float power, int bounces, float scale) : Elastic(value, power,
                                                                                     bounces,
                                                                                     scale) {
            }

            float apply(float a) {
                if (a == 0) return 0;
                a = 1 - a;
                return (1 - (float) bx::pow(value, power * (a - 1)) * bx::sin(a * bounces) * scale);
            }
        };

        class BounceOut : public Interpolation {
        protected:
            float *widths;
            float *heights;
            int count;
        public:
            BounceOut(float *_widths, float *_heights, int _count) : count(_count) {
                int len = sizeof(float) * _count;
                widths = (float *) malloc(len);
                heights = (float *) malloc(len);
                memcpy(widths, _widths, len);
                memcpy(heights, _heights, len);
            }

            ~BounceOut() {
                if (widths) {
                    free(widths);
                    widths = nullptr;
                }
                if (heights) {
                    free(heights);
                    heights = nullptr;
                }
            }

            //bounces should be in [2, 5]
            BounceOut(int bounces) : count(bounces) {
                int len = sizeof(float) * bounces;
                widths = (float *) malloc(len);
                heights = (float *) malloc(len);
                heights[0] = 1;
                switch (bounces) {
                    case 2:
                        widths[0] = 0.6f;
                        widths[1] = 0.4f;
                        heights[1] = 0.33f;
                        break;
                    case 3:
                        widths[0] = 0.4f;
                        widths[1] = 0.4f;
                        widths[2] = 0.2f;
                        heights[1] = 0.33f;
                        heights[2] = 0.1f;
                        break;
                    case 4:
                        widths[0] = 0.34f;
                        widths[1] = 0.34f;
                        widths[2] = 0.2f;
                        widths[3] = 0.15f;
                        heights[1] = 0.26f;
                        heights[2] = 0.11f;
                        heights[3] = 0.03f;
                        break;
                    case 5:
                        widths[0] = 0.3f;
                        widths[1] = 0.3f;
                        widths[2] = 0.2f;
                        widths[3] = 0.1f;
                        widths[4] = 0.1f;
                        heights[1] = 0.45f;
                        heights[2] = 0.3f;
                        heights[3] = 0.15f;
                        heights[4] = 0.06f;
                        break;
                }
                widths[0] *= 2;
            }

            float apply(float a) {
                if (a == 1) return 1;
                a += widths[0] / 2;
                float width = 0, height = 0;
                for (int i = 0, n = count; i < n; i++) {
                    width = widths[i];
                    if (a <= width) {
                        height = heights[i];
                        break;
                    }
                    a -= width;
                }
                a /= width;
                float z = 4 / width * height * a;
                return 1 - (z - z * a) * width;
            }
        };

        class BounceIn : public BounceOut {
        public:
            BounceIn(float *widths, float *heights, int count) : BounceOut(widths, heights,
                                                                           count) {}

            BounceIn(int bounces) : BounceOut(bounces) {}

            float apply(float a) {
                return 1 - BounceOut::apply(1 - a);
            }
        };

        class Bounce : public BounceOut {
        public:
            Bounce(float *widths, float *heights, int count) : BounceOut(widths, heights,
                                                                         count) {}

            Bounce(int bounces) : BounceOut(bounces) {}

        private:
            inline float out(float a) {
                float test = a + widths[0] / 2;
                if (test < widths[0]) return test / (widths[0] / 2) - 1;
                return BounceOut::apply(a);
            }

        public:
            float apply(float a) {
                if (a <= 0.5f) return (1 - out(1 - a * 2)) / 2;
                return out(a * 2 - 1) / 2 + 0.5f;
            }
        };

        class Swing : public Interpolation {
        protected:
            float scale;

        public:
            Swing(float scale) {
                this->scale = scale * 2;
            }

            float apply(float a) {
                if (a <= 0.5f) {
                    a *= 2;
                    return a * a * ((scale + 1) * a - scale) / 2;
                }
                a--;
                a *= 2;
                return a * a * ((scale + 1) * a + scale) / 2 + 1;
            }
        };

        class SwingOut : public Swing {
        public:
            SwingOut(float scale) : Swing(scale) {}

            float apply(float a) {
                a--;
                return a * a * ((scale + 1) * a + scale) + 1;
            }
        };

        class SwingIn : public Swing {
        public:
            SwingIn(float scale) : Swing(scale) {}

            float apply(float a) {
                return a * a * ((scale + 1) * a - scale);
            }
        };

        static Interpolation *_linear_ins = nullptr;

        Interpolation *getlinear() {
            if (_linear_ins == nullptr) { _linear_ins = new Interpolation(); }
            return _linear_ins;
        }

        // DEF_INTERPOLATION2(linear, a)
        class smooth : public Interpolation {
        public:
            virtual float apply(float a) {
                ;
                return a * a * (3 - 2 * a);
            }
        };

        static smooth *_smooth_ins = nullptr;

        Interpolation *getsmooth() {
            if (_smooth_ins == nullptr) { _smooth_ins = new smooth(); }
            return _smooth_ins;
        }

        //DEF_INTERPOLATION2(smooth, a * a * (3 - 2 * a))
        class smooth2 : public Interpolation {
        public:
            virtual float apply(float a) {
                a = a * a * (3 - 2 * a);
                return a * a * (3 - 2 * a);
            }
        };

        static smooth2 *_smooth2_ins = nullptr;

        Interpolation *getsmooth2() {
            if (_smooth2_ins == nullptr) { _smooth2_ins = new smooth2(); }
            return _smooth2_ins;
        }

        //DEF_INTERPOLATION(smooth2, a = a * a * (3 - 2 * a), a * a * (3 - 2 * a))
        class fade : public Interpolation {
        public:
            virtual float apply(float a) {
                ;
                return a * a * a * (a * (a * 6 - 15) + 10);
            }
        };

        static fade *_fade_ins = nullptr;

        Interpolation *getfade() {
            if (_fade_ins == nullptr) { _fade_ins = new fade(); }
            return _fade_ins;
        }

        // DEF_INTERPOLATION2(fade, a * a * a * (a * (a * 6 - 15) + 10))
        class pow3InInverse : public Interpolation {
        public:
            virtual float apply(float a) {
                ;
                return std::cbrt(a);
            }
        };

        static pow3InInverse *_pow3InInverse_ins = nullptr;

        Interpolation *getpow3InInverse() {
            if (_pow3InInverse_ins == nullptr) { _pow3InInverse_ins = new pow3InInverse(); }
            return _pow3InInverse_ins;
        }

        //DEF_INTERPOLATION2(pow3InInverse, std::cbrt(a))
        class pow3OutInverse : public Interpolation {
        public:
            virtual float apply(float a) {
                ;
                return 1 - std::cbrt(a);
            }
        };

        static pow3OutInverse *_pow3OutInverse_ins = nullptr;

        Interpolation *getpow3OutInverse() {
            if (_pow3OutInverse_ins == nullptr) { _pow3OutInverse_ins = new pow3OutInverse(); }
            return _pow3OutInverse_ins;
        }

        //DEF_INTERPOLATION2(pow3OutInverse, 1 - std::cbrt(a))
        class sine : public Interpolation {
        public:
            virtual float apply(float a) {
                ;
                return (1 - bx::cos(a * bx::kPi)) / 2;
            }
        };

        static sine *_sine_ins = nullptr;

        Interpolation *getsine() {
            if (_sine_ins == nullptr) { _sine_ins = new sine(); }
            return _sine_ins;
        }

        //DEF_INTERPOLATION2(sine, (1 - bx::cos(a * bx::kPi)) / 2)
        class sineIn : public Interpolation {
        public:
            virtual float apply(float a) {
                ;
                return 1 - bx::cos(a * bx::kPiHalf);
            }
        };

        static sineIn *_sineIn_ins = nullptr;

        Interpolation *getsineIn() {
            if (_sineIn_ins == nullptr) { _sineIn_ins = new sineIn(); }
            return _sineIn_ins;
        }

        //DEF_INTERPOLATION2(sineIn, 1 - bx::cos(a * bx::kPiHalf))
        class sineOut : public Interpolation {
        public:
            virtual float apply(float a) {
                ;
                return bx::sin(a * bx::kPiHalf);
            }
        };

        static sineOut *_sineOut_ins = nullptr;

        Interpolation *getsineOut() {
            if (_sineOut_ins == nullptr) { _sineOut_ins = new sineOut(); }
            return _sineOut_ins;
        }

        //DEF_INTERPOLATION2(sineOut, bx::sin(a * bx::kPiHalf))
        class circle : public Interpolation {
        public:
            virtual float apply(float a) {
                if (a <= 0.5f) {
                    a *= 2;
                    return (1 - bx::sqrt(1 - a * a)) / 2;
                }
                a--;
                a *= 2;
                return (bx::sqrt(1 - a * a) + 1) / 2;
            }
        };

        static circle *_circle_ins = nullptr;

        Interpolation *getcircle() {
            if (_circle_ins == nullptr) { _circle_ins = new circle(); }
            return _circle_ins;
        }

        /*DEF_INTERPOLATION(circle, if (a <= 0.5f) {
            a *= 2;
            return (1 - bx::sqrt(1 - a * a)) / 2;
        }
                a--;
                a *= 2, (bx::sqrt(1 - a * a) + 1) / 2)*/
        class circleIn : public Interpolation {
        public:
            virtual float apply(float a) {
                ;
                return 1 - bx::sqrt(1 - a * a);
            }
        };

        static circleIn *_circleIn_ins = nullptr;

        Interpolation *getcircleIn() {
            if (_circleIn_ins == nullptr) { _circleIn_ins = new circleIn(); }
            return _circleIn_ins;
        }

        // DEF_INTERPOLATION2(circleIn, 1 - bx::sqrt(1 - a * a))
        class circleOut : public Interpolation {
        public:
            virtual float apply(float a) {
                a--;
                return bx::sqrt(1 - a * a);
            }
        };

        static circleOut *_circleOut_ins = nullptr;

        Interpolation *getcircleOut() {
            if (_circleOut_ins == nullptr) { _circleOut_ins = new circleOut(); }
            return _circleOut_ins;
        }

        // DEF_INTERPOLATION(circleOut, a--, bx::sqrt(1 - a * a))
        class pow2InInverse : public Interpolation {
        public:
            virtual float apply(float a) {
                if (a < 0.000001f)return 0;
                return bx::sqrt(a);
            }
        };

        static pow2InInverse *_pow2InInverse_ins = nullptr;

        Interpolation *getpow2InInverse() {
            if (_pow2InInverse_ins == nullptr) { _pow2InInverse_ins = new pow2InInverse(); }
            return _pow2InInverse_ins;
        }

        //DEF_INTERPOLATION(pow2InInverse, if (a < FLOAT_ROUNDING_ERROR) return 0, bx::sqrt(a))
        class pow2OutInverse : public Interpolation {
        public:
            virtual float apply(float a) {
                if (a < 0.000001f)return 0;
                return 1 - bx::sqrt(-(a - 1));
            }
        };

        static pow2OutInverse *_pow2OutInverse_ins = nullptr;

        Interpolation *getpow2OutInverse() {
            if (_pow2OutInverse_ins == nullptr) { _pow2OutInverse_ins = new pow2OutInverse(); }
            return _pow2OutInverse_ins;
        }

        /* DEF_INTERPOLATION(pow2OutInverse, if (a < FLOAT_ROUNDING_ERROR) return 0,
                           1 - bx::sqrt(-(a - 1))
         }*/
        static Elastic *elastic = nullptr;

        Interpolation *getelastic() {
            if (elastic == nullptr) {
                elastic = new Elastic(2.0f, 10.0f, 7, 1.0f);
            }
            return elastic;
        }

        //DEF_GET_FIELD(Elastic, elastic, new Elastic(2.0f, 10.0f, 7, 1.0f))
        static ElasticIn *elasticIn = nullptr;

        Interpolation *getelasticIn() {
            if (elasticIn == nullptr) {
                elasticIn = new ElasticIn(2.0f, 10.0f, 6, 1.0f);
            }
            return elasticIn;
        }

        //DEF_GET_FIELD(ElasticIn, elasticIn, new ElasticIn(2.0f, 10.0f, 6, 1.0f))
        static ElasticOut *elasticOut = nullptr;

        Interpolation *getelasticOut() {
            if (elasticOut == nullptr) {
                elasticOut = new ElasticOut(2.0f, 10.0f, 7, 1.0f);
            }
            return elasticOut;
        }

        //DEF_GET_FIELD(ElasticOut, elasticOut, new ElasticOut(2.0f, 10.0f, 7, 1.0f))
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

        //DEF_GET_FIELD(PowerOut, pow5Out, new PowerOut(5))
        Interpolation *getslowFast() { return getpow2In(); }
        //GET_FIELD(PowerIn, pow2In, slowFast)

        Interpolation *getfastSlow() { return getpow2Out(); }
        //GET_FIELD(PowerOut, pow2Out, fastSlow)

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
    };
}

#endif //BGFX_STUDY_INTERPOLATION_H
