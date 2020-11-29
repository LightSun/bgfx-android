//
// Created by Administrator on 2020/11/29 0029.
//

#ifndef BGFX_STUDY_MATRIX3F_H
#define BGFX_STUDY_MATRIX3F_H

namespace sb {
    class StringBuilder;
}

namespace h7 {

/**
 * Contains the definition of a 3x3 matrix of floats, and associated functions to transform
 * it. The matrix is column-major to match OpenGL's interpretation, and it looks like this:
 * <p>
 *      m00  m10  m20<br>
 *      m01  m11  m21<br>
 *      m02  m12  m22<br>
 *
 * @author Richard Greenlees
 * @author Kai Burjack
 */
    class Matrix3f {
    private:
        float m00, m01, m02;
        float m10, m11, m12;
        float m20, m21, m22;
    public:
        /**
            * Create a new {@link Matrix3f} and set it to {@link #identity() identity}.
            */
        Matrix3f() {
            m00 = 1.0f;
            m11 = 1.0f;
            m22 = 1.0f;
        }

/**
     * Create a new {@link Matrix3f} and make it a copy of the given matrix.
     *
     * @param mat
     *          the {@link Matrix3fc} to copy the values from
     */
        Matrix3f(Matrix3f &mat) {
            set(mat);
        }

        /**
     * Create a new 3x3 matrix using the supplied float values. The order of the parameter is column-major,
     * so the first three parameters specify the three elements of the first column.
     *
     * @param m00
     *          the value of m00
     * @param m01
     *          the value of m01
     * @param m02
     *          the value of m02
     * @param m10
     *          the value of m10
     * @param m11
     *          the value of m11
     * @param m12
     *          the value of m12
     * @param m20
     *          the value of m20
     * @param m21
     *          the value of m21
     * @param m22
     *          the value of m22
     */
        Matrix3f(float m00, float m01, float m02,
                 float m10, float m11, float m12,
                 float m20, float m21, float m22) {
            set(m00, m01, m02, m10, m11, m12, m20, m21, m22);
        }
        inline Matrix3f &set(float m00, float m01, float m02,
                             float m10, float m11, float m12,
                             float m20, float m21, float m22) {
            this->m00 = m00;
            this->m01 = m01;
            this->m02 = m02;
            this->m10 = m10;
            this->m11 = m11;
            this->m12 = m12;
            this->m20 = m20;
            this->m21 = m21;
            this->m22 = m22;
            return *this;
        }
        inline Matrix3f &set(Matrix3f &mat) {
            m00 = mat.m00;
            m01 = mat.m01;
            m02 = mat.m02;
            m10 = mat.m10;
            m11 = mat.m11;
            m12 = mat.m12;
            m20 = mat.m20;
            m21 = mat.m21;
            m22 = mat.m22;
            return *this;
        }

        inline float _m00() {
            return m00;
        }

        inline float _m01() {
            return m01;
        }

        inline float _m02() {
            return m02;
        }

        inline float _m10() {
            return m10;
        }

        inline float _m11() {
            return m11;
        }

        inline float _m12() {
            return m12;
        }

        inline float _m20() {
            return m20;
        }

        inline float _m21() {
            return m21;
        }

        inline float _m22() {
            return m22;
        }

#define METHOD_FIELD(n)\
        inline Matrix3f& _##n(float n) {\
            this->n = n;\
            return *this;\
        }
        /**
         * Set the value of the matrix element at column 0 and row 0.
         *
         * @param m00
         *          the new value
         * @return this
         */
        METHOD_FIELD(m00)
        /**
         * Set the value of the matrix element at column 0 and row 1.
         *
         * @param m01
         *          the new value
         * @return this
         */
        METHOD_FIELD(m01)
        /**
         * Set the value of the matrix element at column 0 and row 2.
         *
         * @param m02
         *          the new value
         * @return this
         */
        METHOD_FIELD(m02)
        /**
         * Set the value of the matrix element at column 1 and row 0.
         *
         * @param m10
         *          the new value
         * @return this
         */
        METHOD_FIELD(m10)
        /**
         * Set the value of the matrix element at column 1 and row 1.
         *
         * @param m11
         *          the new value
         * @return this
         */
        METHOD_FIELD(m11)
        /**
         * Set the value of the matrix element at column 1 and row 2.
         *
         * @param m12
         *          the new value
         * @return this
         */
        METHOD_FIELD(m12)
        /**
         * Set the value of the matrix element at column 2 and row 0.
         *
         * @param m20
         *          the new value
         * @return this
         */
        METHOD_FIELD(m20)
        /**
         * Set the value of the matrix element at column 2 and row 1.
         *
         * @param m21
         *          the new value
         * @return this
         */
        METHOD_FIELD(m21)
        /**
         * Set the value of the matrix element at column 2 and row 2.
         *
         * @param m22
         *          the new value
         * @return this
         */
        METHOD_FIELD(m22)

        //offset
        bool get(float *dest, int len, int off) {
            if (len - off < 9) {
                return false;
            }
            dest[off + 0] = _m00();
            dest[off + 1] = _m01();
            dest[off + 2] = _m02();
            dest[off + 3] = _m10();
            dest[off + 4] = _m11();
            dest[off + 5] = _m12();
            dest[off + 6] = _m20();
            dest[off + 7] = _m21();
            dest[off + 8] = _m22();
            return true;
        }

        void get(float dest[9]) {
            get(dest, 9, 0);
        }

        /**
    * Get the current values of <code>this</code> matrix and store them into
    * <code>dest</code>.
    * <p>
    * This is the reverse method of {@link #set(Matrix3fc)} and allows to obtain
    * intermediate calculation results when chaining multiple transformations.
    *
    * @see #set(Matrix3fc)
    *
    * @param dest
    *          the destination matrix
    * @return the passed in destination
    */
        inline Matrix3f &get(Matrix3f &dest) {
            return dest.set(*this);
        }

        /**
    * Invert this matrix.
    *
    * @return this
    */
        inline Matrix3f &invert() {
            return invert(*this);
        }

        inline Matrix3f &invert(Matrix3f &dest) {
            float s = 1.0f / determinant();
            float nm00 = (m11 * m22 - m21 * m12) * s;
            float nm01 = (m21 * m02 - m01 * m22) * s;
            float nm02 = (m01 * m12 - m11 * m02) * s;
            float nm10 = (m20 * m12 - m10 * m22) * s;
            float nm11 = (m00 * m22 - m20 * m02) * s;
            float nm12 = (m10 * m02 - m00 * m12) * s;
            float nm20 = (m10 * m21 - m20 * m11) * s;
            float nm21 = (m20 * m01 - m00 * m21) * s;
            float nm22 = (m00 * m11 - m10 * m01) * s;
            dest.m00 = nm00;
            dest.m01 = nm01;
            dest.m02 = nm02;
            dest.m10 = nm10;
            dest.m11 = nm11;
            dest.m12 = nm12;
            dest.m20 = nm20;
            dest.m21 = nm21;
            dest.m22 = nm22;
            return dest;
        }

        inline float determinant() {
            return (m00 * m11 - m01 * m10) * m22
                   + (m02 * m10 - m00 * m12) * m21
                   + (m01 * m12 - m02 * m11) * m20;
        }

        inline Matrix3f &identity() {
            identity(*this);
            return *this;
        }

        inline Matrix3f &zero() {
            zero(*this);
            return *this;
        }

        inline Matrix3f &scale(float x, float y, float z, Matrix3f &dest) {
            // scale matrix elements:
            // m00 = x, m11 = y, m22 = z
            // all others = 0
            dest.m00 = m00 * x;
            dest.m01 = m01 * x;
            dest.m02 = m02 * x;
            dest.m10 = m10 * y;
            dest.m11 = m11 * y;
            dest.m12 = m12 * y;
            dest.m20 = m20 * z;
            dest.m21 = m21 * z;
            dest.m22 = m22 * z;
            return dest;
        }

        /**
  * Apply scaling to this matrix by scaling the base axes by the given x,
  * y and z factors.
  * <p>
  * If <code>M</code> is <code>this</code> matrix and <code>S</code> the scaling matrix,
  * then the new matrix will be <code>M * S</code>. So when transforming a
  * vector <code>v</code> with the new matrix by using <code>M * S * v</code>
  * , the scaling will be applied first!
  *
  * @param x
  *            the factor of the x component
  * @param y
  *            the factor of the y component
  * @param z
  *            the factor of the z component
  * @return this
  */
        inline Matrix3f &scale(float x, float y, float z) {
            return scale(x, y, z, *this);
        }

        inline Matrix3f &scale(float xyz, Matrix3f dest) {
            return scale(xyz, xyz, xyz, dest);
        }

        /**
         * Apply scaling to this matrix by uniformly scaling all base axes by the given <code>xyz</code> factor.
         * <p>
                 * If <code>M</code> is <code>this</code> matrix and <code>S</code> the scaling matrix,
         * then the new matrix will be <code>M * S</code>. So when transforming a
         * vector <code>v</code> with the new matrix by using <code>M * S * v</code>
         * , the scaling will be applied first!
         *
         * @see #scale(float, float, float)
         *
         * @param xyz
         *            the factor for all components
         * @return this
         */
        inline Matrix3f &scale(float xyz) {
            return scale(xyz, xyz, xyz);
        }
        Matrix3f& transpose(Matrix3f& dest) {
            dest.set(m00, m10, m20,
                     m01, m11, m21,
                     m02, m12, m22);
            return dest;
        }
        /*Matrix3f& transform(float x, float y, float z, Matrix3f &dest){
            dest.set(m00 * x + m10 * y + m20 * z,
                     m01 * x + m11 * y + m21 * z,
                     m02 * x + m12 * y + m22 * z);
            return dest;
        }*/
        static inline void identity(Matrix3f &dest) {
            dest._m00(1.0f);
            dest._m01(0.0f);
            dest._m02(0.0f);
            dest._m10(0.0f);
            dest._m11(1.0f);
            dest._m12(0.0f);
            dest._m20(0.0f);
            dest._m21(0.0f);
            dest._m22(1.0f);
        }
        static inline void zero(Matrix3f &dest) {
            dest._m00(0.0f);
            dest._m01(0.0f);
            dest._m02(0.0f);
            dest._m10(0.0f);
            dest._m11(0.0f);
            dest._m12(0.0f);
            dest._m20(0.0f);
            dest._m21(0.0f);
            dest._m22(0.0f);
        }

        void toString(sb::StringBuilder &sb);
    };
}

#endif //BGFX_STUDY_MATRIX3F_H
