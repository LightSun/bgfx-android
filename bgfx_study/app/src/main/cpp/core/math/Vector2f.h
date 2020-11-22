//
// Created by Administrator on 2020/11/22 0022.
//

#ifndef BGFX_STUDY_VECTOR2F_H
#define BGFX_STUDY_VECTOR2F_H

namespace h7{
    class Vector2f{
    public:
        float x, y;
        Vector2f():Vector2f(0,0){}
        Vector2f(float x, float y) : x(x), y(y) {}
        Vector2f(Vector2f& vec): Vector2f(vec.x, vec.y){}

        Vector2f* cpy(Vector2f* out = nullptr);
        float len();
        float lenSquare();
        Vector2f& set(Vector2f& in);
        Vector2f& set(float x, float y);

        Vector2f& sub(float x, float y);
        Vector2f& sub(Vector2f& in);

        Vector2f& add(float x, float y);
        Vector2f& add(Vector2f& in);

        Vector2f& mul(float x, float y);
        Vector2f& mul(Vector2f& in);

        Vector2f& div(float x, float y);
        Vector2f& div(Vector2f& in);

        Vector2f& mulAdd(Vector2f& in, float scalar);
        Vector2f& mulAdd(Vector2f& in, Vector2f& mulVec);

        Vector2f& nor();

        float dst(Vector2f f);
    };
}

#endif //BGFX_STUDY_VECTOR2F_H
