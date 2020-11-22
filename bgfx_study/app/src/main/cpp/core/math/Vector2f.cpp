//
// Created by Administrator on 2020/11/22 0022.
//

#include "Vector2f.h"
#include <math.h>
namespace h7{

    Vector2f * Vector2f::cpy(Vector2f *out) {
        if(out == nullptr){
            out = new Vector2f(x, y);
        }
        return out;
    }
    Vector2f& Vector2f::nor() {
        float d = len();
        if(d != 0){
            x /= d;
            y /= d;
        }
        return *this;
    }
    float Vector2f::len() {
        return sqrt(x*x + y *y);
    }
    float Vector2f::lenSquare() {
        return x*x + y *y;
    }
    Vector2f & Vector2f::set(Vector2f &in) {
        this->x = in.x;
        this->y = in.y;
        return *this;
    }
    Vector2f & Vector2f::set(float x, float y) {
        this->x = x;
        this->y = y;
        return *this;
    }
    Vector2f & Vector2f::sub(Vector2f &in) {
        this->x -= in.x;
        this->y -= in.y;
        return *this;
    }
    Vector2f & Vector2f::sub(float x, float y) {
        this->x -= x;
        this->y -= y;
        return *this;
    }
    Vector2f & Vector2f::add(Vector2f &in) {
        this->x += in.x;
        this->y += in.y;
        return *this;
    }
    Vector2f & Vector2f::add(float x, float y) {
        this->x += x;
        this->y += y;
        return *this;
    }
    Vector2f & Vector2f::mul(Vector2f &in) {
        this->x *= in.x;
        this->y *= in.y;
        return *this;
    }
    Vector2f & Vector2f::mul(float x, float y) {
        this->x *= x;
        this->y *= y;
        return *this;
    }
    Vector2f & Vector2f::div(Vector2f &in) {
        this->x /= in.x;
        this->y /= in.y;
        return *this;
    }
    Vector2f & Vector2f::div(float x, float y) {
        this->x /= x;
        this->y /= y;
        return *this;
    }
    Vector2f & Vector2f::mulAdd(Vector2f &vec, float scalar) {
        this->x += vec.x * scalar;
        this->y += vec.y * scalar;
        return *this;
    }
    Vector2f & Vector2f::mulAdd(Vector2f &vec, Vector2f &mulVec) {
        this->x += vec.x * mulVec.x;
        this->y += vec.y * mulVec.y;
        return *this;
    }

    float Vector2f::dst(Vector2f f) {
         float x_d = x - this->x;
         float y_d = y - this->y;
        return (float)sqrt(x_d * x_d + y_d * y_d);
    }
}