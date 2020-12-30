//
// Created by Administrator on 2020/12/3 0003.
//

#ifndef BGFX_STUDY_INTERPOLATION_H
#define BGFX_STUDY_INTERPOLATION_H

#pragma once


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
        //from 0~1 => 0~1
        virtual float apply(float a) {
            return a;
        }

        /** normalize the apply result to a range. [start, end] */
        inline float apply(float start, float end, float a) {
            return start + (end - start) * apply(a);
        }

    };
    namespace Interpolations {

        Interpolation *getlinear();

        // DEF_INTERPOLATION2(linear, a)

        Interpolation *getsmooth();

        //DEF_INTERPOLATION2(smooth, a * a * (3 - 2 * a))
        Interpolation *getsmooth2();

        //DEF_INTERPOLATION(smooth2, a = a * a * (3 - 2 * a), a * a * (3 - 2 * a))

        Interpolation *getfade();

        // DEF_INTERPOLATION2(fade, a * a * a * (a * (a * 6 - 15) + 10))

        Interpolation *getpow3InInverse();

        //DEF_INTERPOLATION2(pow3InInverse, std::cbrt(a))

        Interpolation *getpow3OutInverse();

        //DEF_INTERPOLATION2(pow3OutInverse, 1 - std::cbrt(a))

        Interpolation *getsine();

        //DEF_INTERPOLATION2(sine, (1 - bx::cos(a * bx::kPi)) / 2)
        Interpolation *getsineIn();

        //DEF_INTERPOLATION2(sineIn, 1 - bx::cos(a * bx::kPiHalf))

        Interpolation *getsineOut();

        //DEF_INTERPOLATION2(sineOut, bx::sin(a * bx::kPiHalf))

        Interpolation *getcircle();

        /*DEF_INTERPOLATION(circle, if (a <= 0.5f) {
            a *= 2;
            return (1 - bx::sqrt(1 - a * a)) / 2;
        }
                a--;
                a *= 2, (bx::sqrt(1 - a * a) + 1) / 2)*/
        Interpolation *getcircleIn();

        // DEF_INTERPOLATION2(circleIn, 1 - bx::sqrt(1 - a * a))

        Interpolation *getcircleOut();

        // DEF_INTERPOLATION(circleOut, a--, bx::sqrt(1 - a * a))
        Interpolation *getpow2InInverse();

        //DEF_INTERPOLATION(pow2InInverse, if (a < FLOAT_ROUNDING_ERROR) return 0, bx::sqrt(a))

        Interpolation *getpow2OutInverse();

        /* DEF_INTERPOLATION(pow2OutInverse, if (a < FLOAT_ROUNDING_ERROR) return 0,
                           1 - bx::sqrt(-(a - 1))
         }*/
        Interpolation *getelastic();

        //DEF_GET_FIELD(Elastic, elastic, new Elastic(2.0f, 10.0f, 7, 1.0f))

        Interpolation *getelasticIn() ;

        //DEF_GET_FIELD(ElasticIn, elasticIn, new ElasticIn(2.0f, 10.0f, 6, 1.0f))

        Interpolation *getelasticOut();

        //DEF_GET_FIELD(ElasticOut, elasticOut, new ElasticOut(2.0f, 10.0f, 7, 1.0f))

        Interpolation *getswing();

        //DEF_GET_FIELD(Swing, swing, new Swing(1.5f))

        Interpolation *getswingIn();

        //DEF_GET_FIELD(SwingIn, swingIn, new SwingIn(2.0f))

        Interpolation *getswingOut();

        //DEF_GET_FIELD(SwingOut, swingOut, new SwingOut(2.0f))

        Interpolation *getbounce();

        //DEF_GET_FIELD(Bounce, bounce, new Bounce(4))

        Interpolation *getbounceIn();

        // DEF_GET_FIELD(BounceIn, bounceIn, new BounceIn(4))

        Interpolation *getbounceOut();

        //DEF_GET_FIELD(BounceOut, bounceOut, new BounceOut(4))

        Interpolation *getexp10();

        //DEF_GET_FIELD(Exp, exp10, new Exp(2, 10))

        Interpolation *getexp10In();

        //DEF_GET_FIELD(ExpIn, exp10In, new ExpIn(2, 10))

        Interpolation *getexp10Out();

        // DEF_GET_FIELD(ExpOut, exp10Out, new ExpOut(2, 10))

        Interpolation *getexp5();

        //DEF_GET_FIELD(Exp, exp5, new Exp(2, 5))

        Interpolation *getexp5In() ;

        //DEF_GET_FIELD(ExpIn, exp5In, new ExpIn(2, 5))

        Interpolation *getexp5Out();

        //DEF_GET_FIELD(ExpOut, exp5Out, new ExpOut(2, 5))

        Interpolation *getpow2();

        //DEF_GET_FIELD(Power, pow2, new Power(2))

        Interpolation *getpow2In();

        //DEF_GET_FIELD(PowerIn, pow2In, new PowerIn(2))

        Interpolation *getpow2Out() ;

        //DEF_GET_FIELD(PowerOut, pow2Out, new PowerOut(2))

        Interpolation *getpow3();

        //DEF_GET_FIELD(Power, pow3, new Power(3))

        Interpolation *getpow3In();

        //DEF_GET_FIELD(PowerIn, pow3In, new PowerIn(3))

        Interpolation *getpow3Out();

        //DEF_GET_FIELD(PowerOut, pow3Out, new PowerOut(3))

        Interpolation *getpow4();

        //DEF_GET_FIELD(Power, pow4, new Power(4))

        Interpolation *getpow4In();

        //DEF_GET_FIELD(PowerIn, pow4In, new PowerIn(4))

        Interpolation *getpow4Out();

        //DEF_GET_FIELD(PowerOut, pow4Out, new PowerOut(4))

        Interpolation *getpow5();

        //DEF_GET_FIELD(Power, pow5, new Power(5))

        Interpolation *getpow5In();

        //DEF_GET_FIELD(PowerIn, pow5In, new PowerIn(5))

        Interpolation *getpow5Out();

        //DEF_GET_FIELD(PowerOut, pow5Out, new PowerOut(5))
        Interpolation *getslowFast();
        //GET_FIELD(PowerIn, pow2In, slowFast)

        Interpolation *getfastSlow();
        //GET_FIELD(PowerOut, pow2Out, fastSlow)

        Interpolation *getViscousFluid();
    };
}

#endif //BGFX_STUDY_INTERPOLATION_H
