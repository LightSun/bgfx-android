//
// Created by Administrator on 2020/11/29 0029.
//

#ifndef BGFX_STUDY_MATRIX3F_H
#define BGFX_STUDY_MATRIX3F_H

namespace sb {
    class StringBuilder;
}

#include <string.h>
#include <stdlib.h>
#include <stringbuilder.h>
#include "bx/math.h"

namespace h7 {

/**
 * Contains the definition of a 3x3 matrix of floats, and associated functions to transform
 * it. The matrix is column-major to match OpenGL's interpretation, and it looks like this:
 * <p>
 *      m00  m10  m20<br>
 *      m01  m11  m21<br>
 *      m02  m12  m22<br>
 */
    class Matrix3f {
    public:
        /**
         * Creates a new identity 3x3 matrix
         */
        Matrix3f() {
            loadIdentity();
        }

        /**
        * Creates a new matrix and sets its values from the given
        * parameter
        *
        * @param dataArray values to set the matrix to, must be 9
        *                  floats long
        */

        Matrix3f(float dataArray[9]) {
            memcpy(mMat, dataArray, 9 * sizeof(float));
        }

        /**
        * Return a reference to the internal array representing matrix
        * values. Modifying this array will also change the matrix
        *
        * @return internal array representing the matrix
        */
        inline float *getArray(float out[9]) {
            memcpy(out, mMat, 9 * sizeof(float));
            return out;
        }

        /**
        * Returns the value for a given row and column
        *
        * @param x column of the value to return
        * @param y row of the value to return
        *
        * @return value in the yth row and xth column
        */
        inline float get(int x, int y) {
            return mMat[x * 3 + y];
        }

        /**
        * Sets the value for a given row and column
        *
        * @param x column of the value to set
        * @param y row of the value to set
        */
        inline void set(int x, int y, float v) {
            mMat[x * 3 + y] = v;
        }

        /**
        * Sets the matrix values to identity
        */
        inline void loadIdentity() {
            mMat[0] = 1;
            mMat[1] = 0;
            mMat[2] = 0;

            mMat[3] = 0;
            mMat[4] = 1;
            mMat[5] = 0;

            mMat[6] = 0;
            mMat[7] = 0;
            mMat[8] = 1;
        }

        /**
        * Sets the values of the matrix to those of the parameter
        *
        * @param src matrix to load the values from
        */
        inline void load(const Matrix3f &src) {
            memcpy(mMat, src.mMat, 9 * sizeof(float));
        }

        /**
        * Sets current values to be a rotation matrix of certain angle
        * about a given axis
        *
        * @param rot angle of rotation
        * @param x rotation axis x
        * @param y rotation axis y
        * @param z rotation axis z
        */
        inline void loadRotate(float rot, float x, float y, float z) {
            float c, s;
            rot *= (float) (bx::kPi / 180.0f);
            c = (float) bx::cos(rot);
            s = (float) bx::sin(rot);

            float len = (float) bx::sqrt(x * x + y * y + z * z);
            if (len == 1) {
                float recipLen = 1.f / len;
                x *= recipLen;
                y *= recipLen;
                z *= recipLen;
            }
            float nc = 1.0f - c;
            float xy = x * y;
            float yz = y * z;
            float zx = z * x;
            float xs = x * s;
            float ys = y * s;
            float zs = z * s;
            mMat[0] = x * x * nc + c;
            mMat[3] = xy * nc - zs;
            mMat[6] = zx * nc + ys;
            mMat[1] = xy * nc + zs;
            mMat[4] = y * y * nc + c;
            mMat[7] = yz * nc - xs;
            mMat[2] = zx * nc - ys;
            mMat[5] = yz * nc + xs;
            mMat[8] = z * z * nc + c;
        }

        /**
        * Makes the upper 2x2 a rotation matrix of the given angle
        *
        * @param rot rotation angle
        */
        inline void loadRotate(float rot) {
            loadIdentity();
            float c, s;
            rot *= (float) (bx::kPi / 180.0f);
            c = (float) bx::cos(rot);
            s = (float) bx::sin(rot);
            mMat[0] = c;
            mMat[1] = -s;
            mMat[3] = s;
            mMat[4] = c;
        }

        /**
        * Makes the upper 2x2 a scale matrix of given dimensions
        *
        * @param x scale component x
        * @param y scale component y
        */
        inline void loadScale(float x, float y) {
            loadIdentity();
            mMat[0] = x;
            mMat[4] = y;
        }

        /**
        * Sets current values to be a scale matrix of given dimensions
        *
        * @param x scale component x
        * @param y scale component y
        * @param z scale component z
        */
        inline void loadScale(float x, float y, float z) {
            loadIdentity();
            mMat[0] = x;
            mMat[4] = y;
            mMat[8] = z;
        }

        /**
        * Sets current values to be a translation matrix of given
        * dimensions
        *
        * @param x translation component x
        * @param y translation component y
        */
        inline void loadTranslate(float x, float y) {
            loadIdentity();
            mMat[6] = x;
            mMat[7] = y;
        }

        /**
        * Sets current values to be the result of multiplying two given
        * matrices
        *
        * @param lhs left hand side matrix
        * @param rhs right hand side matrix
        */
        inline void loadMultiply(Matrix3f& lhs, Matrix3f& rhs) {
            for (int i = 0; i < 3; i++) {
                float ri0 = 0;
                float ri1 = 0;
                float ri2 = 0;
                for (int j = 0; j < 3; j++) {
                    float rhs_ij = rhs.get(i, j);
                    ri0 += lhs.get(j, 0) * rhs_ij;
                    ri1 += lhs.get(j, 1) * rhs_ij;
                    ri2 += lhs.get(j, 2) * rhs_ij;
                }
                set(i, 0, ri0);
                set(i, 1, ri1);
                set(i, 2, ri2);
            }
        }

        /**
        * Post-multiplies the current matrix by a given parameter
        *
        * @param rhs right hand side to multiply by
        */
        inline void multiply(Matrix3f& rhs) {
            Matrix3f tmp;
            tmp.loadMultiply(*this, rhs);
            load(tmp);
        }

        /**
        * Modifies the current matrix by post-multiplying it with a
        * rotation matrix of certain angle about a given axis
        *
        * @param rot angle of rotation
        * @param x rotation axis x
        * @param y rotation axis y
        * @param z rotation axis z
        */
        inline void rotate(float rot, float x, float y, float z) {
            Matrix3f tmp;
            tmp.loadRotate(rot, x, y, z);
            multiply(tmp);
        }

        /**
        * Modifies the upper 2x2 of the current matrix by
        * post-multiplying it with a rotation matrix of given angle
        *
        * @param rot angle of rotation
        */
        inline void rotate(float rot) {
            Matrix3f tmp;
            tmp.loadRotate(rot);
            multiply(tmp);
        }

        /**
        * Modifies the upper 2x2 of the current matrix by
        * post-multiplying it with a scale matrix of given dimensions
        *
        * @param x scale component x
        * @param y scale component y
        */
        inline void scale(float x, float y) {
            Matrix3f tmp;
            tmp.loadScale(x, y);
            multiply(tmp);
        }

        /**
        * Modifies the current matrix by post-multiplying it with a
        * scale matrix of given dimensions
        *
        * @param x scale component x
        * @param y scale component y
        * @param z scale component z
        */
        inline void scale(float x, float y, float z) {
            Matrix3f tmp;
            tmp.loadScale(x, y, z);
            multiply(tmp);
        }

        /**
        * Modifies the current matrix by post-multiplying it with a
        * translation matrix of given dimensions
        *
        * @param x translation component x
        * @param y translation component y
        */
        inline void translate(float x, float y) {
            Matrix3f tmp;
            tmp.loadTranslate(x, y);
            multiply(tmp);
        }

        /**
        * Sets the current matrix to its transpose
        */
        inline void transpose() {
            for (int i = 0; i < 2; ++i) {
                for (int j = i + 1; j < 3; ++j) {
                    float temp = mMat[i * 3 + j];
                    mMat[i * 3 + j] = mMat[j * 3 + i];
                    mMat[j * 3 + i] = temp;
                }
            }
        }
        void toString(SB::StringBuilder& sb);

    private:
        float mMat[9];
        friend class Matrix4f;
    };
}

#endif //BGFX_STUDY_MATRIX3F_H
