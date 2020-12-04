//
// Created by Administrator on 2020/12/4 0004.
//

#ifndef BGFX_STUDY_MATRIX4F_H
#define BGFX_STUDY_MATRIX4F_H

#include <string.h>
#include <stdlib.h>
#include "bx/math.h"
#include "Matrix3f.h"

namespace h7 {
    /**
 * matrix4x4
 **/
    class Matrix4f {
    public:

        /**
        * Creates a new identity 4x4 matrix
        */
        Matrix4f() {
            loadIdentity();
        }

        /**
        * Creates a new matrix and sets its values from the given
        * parameter
        *
        * @param dataArray values to set the matrix to, must be 16
        *                  floats long
        */
        Matrix4f(float dataArray[16]) {
            memcpy(mMat, dataArray, 16 * sizeof(float));
        }

        /**
        * Return a reference to the internal array representing matrix
        * values. Modifying this array will also change the matrix
        *
        * @return internal array representing the matrix
        */
        inline float *getArray(float out[16]) {
            memcpy(out, mMat, 16 * sizeof(float));
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
            return mMat[x * 4 + y];
        }

        /**
        * Sets the value for a given row and column
        *
        * @param x column of the value to set
        * @param y row of the value to set
        */
        inline void set(int x, int y, float v) {
            mMat[x * 4 + y] = v;
        }

        /**
        * Sets the matrix values to identity
        */
        inline void loadIdentity() {
            mMat[0] = 1;
            mMat[1] = 0;
            mMat[2] = 0;
            mMat[3] = 0;

            mMat[4] = 0;
            mMat[5] = 1;
            mMat[6] = 0;
            mMat[7] = 0;

            mMat[8] = 0;
            mMat[9] = 0;
            mMat[10] = 1;
            mMat[11] = 0;

            mMat[12] = 0;
            mMat[13] = 0;
            mMat[14] = 0;
            mMat[15] = 1;
        }

        /**
        * Sets the values of the matrix to those of the parameter
        *
        * @param src matrix to load the values from
        */
        inline void load(const Matrix4f &src) {
            memcpy(mMat, src.mMat, 16 * sizeof(float));
        }

        /**
        * Sets the values of the matrix to those of the parameter
        *
        * @param src matrix to load the values from
        * @hide
        */
        inline void load(const Matrix3f& src) {
            mMat[0] = src.mMat[0];
            mMat[1] = src.mMat[1];
            mMat[2] = src.mMat[2];
            mMat[3] = 0;

            mMat[4] = src.mMat[3];
            mMat[5] = src.mMat[4];
            mMat[6] = src.mMat[5];
            mMat[7] = 0;

            mMat[8] = src.mMat[6];
            mMat[9] = src.mMat[7];
            mMat[10] = src.mMat[8];
            mMat[11] = 0;

            mMat[12] = 0;
            mMat[13] = 0;
            mMat[14] = 0;
            mMat[15] = 1;
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
            mMat[3] = 0;
            mMat[7] = 0;
            mMat[11] = 0;
            mMat[12] = 0;
            mMat[13] = 0;
            mMat[14] = 0;
            mMat[15] = 1;
            rot *= (float) (bx::kPi / 180.0f);
            c = (float) bx::cos(rot);
            s = (float) bx::sin(rot);

            float len = (float) bx::sqrt(x * x + y * y + z * z);
            //if (!(len != 1)) {
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
            mMat[4] = xy * nc - zs;
            mMat[8] = zx * nc + ys;
            mMat[1] = xy * nc + zs;
            mMat[5] = y * y * nc + c;
            mMat[9] = yz * nc - xs;
            mMat[2] = zx * nc - ys;
            mMat[6] = yz * nc + xs;
            mMat[10] = z * z * nc + c;
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
            mMat[5] = y;
            mMat[10] = z;
        }

        /**
        * Sets current values to be a translation matrix of given
        * dimensions
        *
        * @param x translation component x
        * @param y translation component y
        * @param z translation component z
        */
        inline void loadTranslate(float x, float y, float z) {
            loadIdentity();
            mMat[12] = x;
            mMat[13] = y;
            mMat[14] = z;
        }

        /**
        * Sets current values to be the result of multiplying two given
        * matrices
        *
        * @param lhs left hand side matrix
        * @param rhs right hand side matrix
        */
        inline void loadMultiply(Matrix4f& lhs, Matrix4f& rhs) {
            for (int i = 0; i < 4; i++) {
                float ri0 = 0;
                float ri1 = 0;
                float ri2 = 0;
                float ri3 = 0;
                for (int j = 0; j < 4; j++) {
                    float rhs_ij = rhs.get(i, j);
                    ri0 += lhs.get(j, 0) * rhs_ij;
                    ri1 += lhs.get(j, 1) * rhs_ij;
                    ri2 += lhs.get(j, 2) * rhs_ij;
                    ri3 += lhs.get(j, 3) * rhs_ij;
                }
                set(i, 0, ri0);
                set(i, 1, ri1);
                set(i, 2, ri2);
                set(i, 3, ri3);
            }
        }

        /**
        * Set current values to be an orthographic projection matrix
        *
        * @param l location of the left vertical clipping plane
        * @param r location of the right vertical clipping plane
        * @param b location of the bottom horizontal clipping plane
        * @param t location of the top horizontal clipping plane
        * @param n location of the near clipping plane
        * @param f location of the far clipping plane
        */
        inline void loadOrtho(float l, float r, float b, float t, float n, float f) {
            loadIdentity();
            mMat[0] = 2 / (r - l);
            mMat[5] = 2 / (t - b);
            mMat[10] = -2 / (f - n);
            mMat[12] = -(r + l) / (r - l);
            mMat[13] = -(t + b) / (t - b);
            mMat[14] = -(f + n) / (f - n);
        }

        /**
        * Set current values to be an orthographic projection matrix
        * with the right and bottom clipping planes set to the given
        * values. Left and top clipping planes are set to 0. Near and
        * far are set to -1, 1 respectively
        *
        * @param w location of the right vertical clipping plane
        * @param h location of the bottom horizontal clipping plane
        *
        */
        inline void loadOrthoWindow(int w, int h) {
            loadOrtho(0, w, h, 0, -1, 1);
        }

        /**
        * Sets current values to be a perspective projection matrix
        *
        * @param l location of the left vertical clipping plane
        * @param r location of the right vertical clipping plane
        * @param b location of the bottom horizontal clipping plane
        * @param t location of the top horizontal clipping plane
        * @param n location of the near clipping plane, must be positive
        * @param f location of the far clipping plane, must be positive
        *
        */
        inline void loadFrustum(float l, float r, float b, float t, float n, float f) {
            loadIdentity();
            mMat[0] = 2 * n / (r - l);
            mMat[5] = 2 * n / (t - b);
            mMat[8] = (r + l) / (r - l);
            mMat[9] = (t + b) / (t - b);
            mMat[10] = -(f + n) / (f - n);
            mMat[11] = -1;
            mMat[14] = -2 * f * n / (f - n);
            mMat[15] = 0;
        }

        /**
        * Sets current values to be a perspective projection matrix
        *
        * @param fovy vertical field of view angle in degrees
        * @param aspect aspect ratio of the screen
        * @param near near cliping plane, must be positive
        * @param far far clipping plane, must be positive
        */
        inline void loadPerspective(float fovy, float aspect, float near, float far) {
            float top = near * (float) bx::tan((float) (fovy * bx::kPi / 360.0f));
            float bottom = -top;
            float left = bottom * aspect;
            float right = top * aspect;
            loadFrustum(left, right, bottom, top, near, far);
        }

        /**
        * Helper function to set the current values to a perspective
        * projection matrix with aspect ratio defined by the parameters
        * and (near, far), (bottom, top) mapping to (-1, 1) at z = 0
        *
        * @param w screen width
        * @param h screen height
        */
        inline void loadProjectionNormalized(int w, int h) {
            // range -1,1 in the narrow axis at z = 0.
            Matrix4f m1;
            Matrix4f m2;

            if (w > h) {
                float aspect = ((float) w) / h;
                m1.loadFrustum(-aspect, aspect, -1, 1, 1, 100);
            } else {
                float aspect = ((float) h) / w;
                m1.loadFrustum(-1, 1, -aspect, aspect, 1, 100);
            }

            m2.loadRotate(180, 0, 1, 0);
            m1.loadMultiply(m1, m2);

            m2.loadScale(-2, 2, 1);
            m1.loadMultiply(m1, m2);

            m2.loadTranslate(0, 0, 2);
            m1.loadMultiply(m1, m2);

            load(m1);
        }

        /**
        * Post-multiplies the current matrix by a given parameter
        *
        * @param rhs right hand side to multiply by
        */
        inline void multiply(Matrix4f rhs) {
            Matrix4f tmp;
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
            Matrix4f tmp;
            tmp.loadRotate(rot, x, y, z);
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
            Matrix4f tmp;
            tmp.loadScale(x, y, z);
            multiply(tmp);
        }

        /**
        * Modifies the current matrix by post-multiplying it with a
        * translation matrix of given dimensions
        *
        * @param x translation component x
        * @param y translation component y
        * @param z translation component z
        */
        inline void translate(float x, float y, float z) {
            Matrix4f tmp;
            tmp.loadTranslate(x, y, z);
            multiply(tmp);
        }

        /**
        * Sets the current matrix to its inverse
        */
        inline bool inverse() {
            Matrix4f result;

            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    result.mMat[4 * i + j] = computeCofactor(i, j);
                }
            }

            // Dot product of 0th column of source and 0th row of result
            float det = mMat[0] * result.mMat[0] + mMat[4] * result.mMat[1] +
                        mMat[8] * result.mMat[2] + mMat[12] * result.mMat[3];

            if (bx::abs(det) < 1e-6) {
                return false;
            }

            det = 1.0f / det;
            for (int i = 0; i < 16; ++i) {
                mMat[i] = result.mMat[i] * det;
            }

            return true;
        }

        /**
        * Sets the current matrix to its inverse transpose
        */

        inline bool inverseTranspose() {

            Matrix4f result;

            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    result.mMat[4 * j + i] = computeCofactor(i, j);
                }
            }

            float det = mMat[0] * result.mMat[0] + mMat[4] * result.mMat[4] +
                        mMat[8] * result.mMat[8] + mMat[12] * result.mMat[12];

            if (bx::abs(det) < 1e-6) {
                return false;
            }

            det = 1.0f / det;
            for (int i = 0; i < 16; ++i) {
                mMat[i] = result.mMat[i] * det;
            }

            return true;
        }

        /**
        * Sets the current matrix to its transpose
        */
        inline void transpose() {
            for (int i = 0; i < 3; ++i) {
                for (int j = i + 1; j < 4; ++j) {
                    float temp = mMat[i * 4 + j];
                    mMat[i * 4 + j] = mMat[j * 4 + i];
                    mMat[j * 4 + i] = temp;
                }
            }
        }

        void toString(SB::StringBuilder& sb);

    private:
        inline float computeCofactor(int i, int j) {
            int c0 = (i + 1) % 4;
            int c1 = (i + 2) % 4;
            int c2 = (i + 3) % 4;
            int r0 = (j + 1) % 4;
            int r1 = (j + 2) % 4;
            int r2 = (j + 3) % 4;

            float minor = (mMat[c0 + 4 * r0] * (mMat[c1 + 4 * r1] * mMat[c2 + 4 * r2] -
                                                mMat[c1 + 4 * r2] * mMat[c2 + 4 * r1]))
                          - (mMat[c0 + 4 * r1] * (mMat[c1 + 4 * r0] * mMat[c2 + 4 * r2] -
                                                  mMat[c1 + 4 * r2] * mMat[c2 + 4 * r0]))
                          + (mMat[c0 + 4 * r2] * (mMat[c1 + 4 * r0] * mMat[c2 + 4 * r1] -
                                                  mMat[c1 + 4 * r1] * mMat[c2 + 4 * r0]));

            float cofactor = ((i + j) & 1) != 0 ? -minor : minor;
            return cofactor;
        }

        float mMat[16];
        friend class Matrix3f;
    };

}

#endif //BGFX_STUDY_MATRIX4F_H
