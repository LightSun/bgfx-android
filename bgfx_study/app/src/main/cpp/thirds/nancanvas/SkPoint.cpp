/*
 * Copyright 2008 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkPoint.h"
#include "../lua/SkScalar.h"

///////////////////////////////////////////////////////////////////////////////

void SkPoint::scale(SkScalar scale, SkPoint* dst) const {
    SkASSERT(dst);
    dst->set(fX * scale, fY * scale);
}

bool SkPoint::normalize() {
    return this->setLength(fX, fY, SK_Scalar1);
}

bool SkPoint::setNormalize(SkScalar x, SkScalar y) {
    return this->setLength(x, y, SK_Scalar1);
}

bool SkPoint::setLength(SkScalar length) {
    return this->setLength(fX, fY, length);
}

/*
 *  We have to worry about 2 tricky conditions:
 *  1. underflow of mag2 (compared against nearlyzero^2)
 *  2. overflow of mag2 (compared w/ isfinite)
 *
 *  If we underflow, we return false. If we overflow, we compute again using
 *  doubles, which is much slower (3x in a desktop test) but will not overflow.
 */
template <bool use_rsqrt> bool set_point_length(SkPoint* pt, float x, float y, float length,
                                                float* orig_length = nullptr) {
    SkASSERT(!use_rsqrt || (orig_length == nullptr));

    // our mag2 step overflowed to infinity, so use doubles instead.
    // much slower, but needed when x or y are very large, other wise we
    // divide by inf. and return (0,0) vector.
    double xx = x;
    double yy = y;
    double dmag = sqrt(xx * xx + yy * yy);
    double dscale = sk_ieee_double_divide(length, dmag);
    x *= dscale;
    y *= dscale;
    // check if we're not finite, or we're zero-length
    if (!sk_float_isfinite(x) || !sk_float_isfinite(y) || (x == 0 && y == 0)) {
        pt->set(0, 0);
        return false;
    }
    float mag = 0;
    if (orig_length) {
        mag = sk_double_to_float(dmag);
    }
    pt->set(x, y);
    if (orig_length) {
        *orig_length = mag;
    }
    return true;
}

SkScalar SkPoint::Normalize(SkPoint* pt) {
    float mag;
    if (set_point_length<false>(pt, pt->fX, pt->fY, 1.0f, &mag)) {
        return mag;
    }
    return 0;
}

SkScalar SkPoint::Length(SkScalar dx, SkScalar dy) {
    float mag2 = dx * dx + dy * dy;
    if (SkScalarIsFinite(mag2)) {
        return sk_float_sqrt(mag2);
    } else {
        double xx = dx;
        double yy = dy;
        return sk_double_to_float(sqrt(xx * xx + yy * yy));
    }
}

bool SkPoint::setLength(float x, float y, float length) {
    return set_point_length<false>(this, x, y, length);
}