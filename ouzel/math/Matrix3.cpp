// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include <cmath>
#include <cassert>
#include "Matrix3.h"
#include "MathUtils.h"

namespace ouzel
{
    const Matrix3 Matrix3::IDENTITY(1.0f, 0.0f, 0.0f,
                                    0.0f, 1.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f);

    const Matrix3 Matrix3::ZERO(0.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 0.0f);

    Matrix3::Matrix3(float pm11, float pm12, float pm13,
                     float pm21, float pm22, float pm23,
                     float pm31, float pm32, float pm33)
    {
        set(pm11, pm12, pm13, pm21, pm22, pm23, pm31, pm32, pm33);
    }

    Matrix3::Matrix3(const float* array)
    {
        set(array);
    }

    Matrix3::Matrix3(const Matrix3& copy)
    {
        std::copy(copy.m, copy.m + sizeof(copy.m) / sizeof(float), m);
    }

    void Matrix3::createScale(const Vector2& scale, Matrix3& dst)
    {
        std::copy(IDENTITY.m, IDENTITY.m + sizeof(IDENTITY.m) / sizeof(float), dst.m);

        dst.m[0] = scale.v[0];
        dst.m[4] = scale.v[1];
    }

    void Matrix3::createScale(float xScale, float yScale, Matrix3& dst)
    {
        std::copy(IDENTITY.m, IDENTITY.m + sizeof(IDENTITY.m) / sizeof(float), dst.m);

        dst.m[0] = xScale;
        dst.m[4] = yScale;
    }

    void Matrix3::createRotation(float angle, Matrix3& dst)
    {
        float c = cosf(angle);
        float s = sinf(angle);

        dst.m[0] = c;
        dst.m[1] = -s;
        dst.m[2] = 0.0f;

        dst.m[3] = s;
        dst.m[4] = c;
        dst.m[5] = 0.0f;

        dst.m[6] = 0.0f;
        dst.m[7] = 0.0f;
        dst.m[8] = 1.0f;
    }

    void Matrix3::createTranslation(const Vector2& translation, Matrix3& dst)
    {
        std::copy(IDENTITY.m, IDENTITY.m + sizeof(IDENTITY.m) / sizeof(float), dst.m);

        dst.m[6] = translation.v[0];
        dst.m[7] = translation.v[1];
    }

    void Matrix3::createTranslation(float xTranslation, float yTranslation, Matrix3& dst)
    {
        std::copy(IDENTITY.m, IDENTITY.m + sizeof(IDENTITY.m) / sizeof(float), dst.m);

        dst.m[6] = xTranslation;
        dst.m[7] = yTranslation;
    }

    void Matrix3::add(float scalar)
    {
        add(scalar, *this);
    }

    void Matrix3::add(float scalar, Matrix3& dst)
    {
        dst.m[0]  = m[0]  + scalar;
        dst.m[1]  = m[1]  + scalar;
        dst.m[2]  = m[2]  + scalar;
        dst.m[3]  = m[3]  + scalar;
        dst.m[4]  = m[4]  + scalar;
        dst.m[5]  = m[5]  + scalar;
        dst.m[6]  = m[6]  + scalar;
        dst.m[7]  = m[7]  + scalar;
        dst.m[8]  = m[8]  + scalar;
    }

    void Matrix3::add(const Matrix3& matrix)
    {
        add(*this, matrix, *this);
    }

    void Matrix3::add(const Matrix3& m1, const Matrix3& m2, Matrix3& dst)
    {
        dst.m[0]  = m1.m[0]  + m2.m[0];
        dst.m[1]  = m1.m[1]  + m2.m[1];
        dst.m[2]  = m1.m[2]  + m2.m[2];
        dst.m[3]  = m1.m[3]  + m2.m[3];
        dst.m[4]  = m1.m[4]  + m2.m[4];
        dst.m[5]  = m1.m[5]  + m2.m[5];
        dst.m[6]  = m1.m[6]  + m2.m[6];
        dst.m[7]  = m1.m[7]  + m2.m[7];
        dst.m[8]  = m1.m[8]  + m2.m[8];
    }

    float Matrix3::determinant() const
    {
        float a0 = m[0] * m[4] * m[8];
        float a1 = m[1] * m[5] * m[6];
        float a2 = m[2] * m[3] * m[7];
        float a3 = m[2] * m[4] * m[6];
        float a4 = m[1] * m[3] * m[8];
        float a5 = m[0] * m[5] * m[7];

        // Calculate the determinant.
        return a0 + a1 + a2 - a3 - a4 - a5;
    }

    bool Matrix3::invert()
    {
        return invert(*this);
    }

    bool Matrix3::invert(Matrix3& dst) const
    {
        float det = +m[0] * (m[4] * m[8] - m[7] * m[5])
                    -m[1] * (m[3] * m[8] - m[5] * m[6])
                    +m[2] * (m[3] * m[7] - m[4] * m[6]);

        // Close to zero, can't invert.
        if (fabs(det) <= TOLERANCE)
            return false;

        float invdet = 1.0f / det;

        // Support the case where m == dst.
        dst.m[0] =  (m[4] * m[8] - m[7] * m[5]) * invdet;
        dst.m[3] = -(m[1] * m[8] - m[2] * m[7]) * invdet;
        dst.m[6] =  (m[1] * m[5] - m[2] * m[4]) * invdet;
        dst.m[1] = -(m[3] * m[8] - m[5] * m[6]) * invdet;
        dst.m[4] =  (m[0] * m[8] - m[2] * m[6]) * invdet;
        dst.m[7] = -(m[0] * m[5] - m[3] * m[2]) * invdet;
        dst.m[2] =  (m[3] * m[7] - m[6] * m[4]) * invdet;
        dst.m[5] = -(m[0] * m[7] - m[6] * m[1]) * invdet;
        dst.m[8] =  (m[0] * m[4] - m[3] * m[1]) * invdet;

        return true;
    }

    bool Matrix3::isIdentity() const
    {
        return std::equal(m, m + sizeof(m) / sizeof(float), IDENTITY.m);
    }

    void Matrix3::multiply(float scalar)
    {
        multiply(scalar, *this);
    }

    void Matrix3::multiply(float scalar, Matrix3& dst) const
    {
        multiply(*this, scalar, dst);
    }

    void Matrix3::multiply(const Matrix3& matrix, float scalar, Matrix3& dst)
    {
        dst.m[0]  = matrix.m[0]  * scalar;
        dst.m[1]  = matrix.m[1]  * scalar;
        dst.m[2]  = matrix.m[2]  * scalar;
        dst.m[3]  = matrix.m[3]  * scalar;
        dst.m[4]  = matrix.m[4]  * scalar;
        dst.m[5]  = matrix.m[5]  * scalar;
        dst.m[6]  = matrix.m[6]  * scalar;
        dst.m[7]  = matrix.m[7]  * scalar;
        dst.m[8]  = matrix.m[8]  * scalar;
    }

    void Matrix3::multiply(const Matrix3& matrix)
    {
        multiply(*this, matrix, *this);
    }

    void Matrix3::multiply(const Matrix3& m1, const Matrix3& m2, Matrix3& dst)
    {
        // Support the case where m1 or m2 is the same array as dst.
        float product[9];

        product[0]  = m1.m[0] * m2.m[0]  + m1.m[3] * m2.m[1] + m1.m[6]  * m2.m[2];
        product[1]  = m1.m[1] * m2.m[0]  + m1.m[4] * m2.m[1] + m1.m[7]  * m2.m[2];
        product[2]  = m1.m[2] * m2.m[0]  + m1.m[5] * m2.m[1] + m1.m[8]  * m2.m[2];

        product[3]  = m1.m[0] * m2.m[3]  + m1.m[3] * m2.m[4] + m1.m[6]  * m2.m[5];
        product[4]  = m1.m[1] * m2.m[3]  + m1.m[4] * m2.m[4] + m1.m[7]  * m2.m[5];
        product[5]  = m1.m[2] * m2.m[3]  + m1.m[5] * m2.m[4] + m1.m[8]  * m2.m[5];

        product[6]  = m1.m[0] * m2.m[6]  + m1.m[3] * m2.m[7] + m1.m[6]  * m2.m[8];
        product[7]  = m1.m[1] * m2.m[6]  + m1.m[4] * m2.m[7] + m1.m[7]  * m2.m[8];
        product[8]  = m1.m[2] * m2.m[6]  + m1.m[5] * m2.m[7] + m1.m[8]  * m2.m[8];

        std::copy(product, product + sizeof(product) / sizeof(float), dst.m);
    }

    void Matrix3::rotate(float angle)
    {
        rotate(angle, *this);
    }

    void Matrix3::rotate(float angle, Matrix3& dst) const
    {
        Matrix3 r;
        createRotation(angle, r);
        multiply(*this, r, dst);
    }

    void Matrix3::scale(float value)
    {
        scale(value, *this);
    }

    void Matrix3::scale(float value, Matrix3& dst) const
    {
        scale(value, value, dst);
    }

    void Matrix3::scale(float xScale, float yScale)
    {
        scale(xScale, yScale, *this);
    }

    void Matrix3::scale(float xScale, float yScale, Matrix3& dst) const
    {
        Matrix3 s;
        createScale(xScale, yScale, s);
        multiply(*this, s, dst);
    }

    void Matrix3::scale(const Vector2& s)
    {
        scale(s.v[0], s.v[1], *this);
    }

    void Matrix3::scale(const Vector2& s, Matrix3& dst) const
    {
        scale(s.v[0], s.v[1], dst);
    }

    void Matrix3::set(float m11, float m12, float m13,
                      float m21, float m22, float m23,
                      float m31, float m32, float m33)
    {
        m[0]  = m11;
        m[1]  = m21;
        m[2]  = m31;
        m[3]  = m12;
        m[4]  = m22;
        m[5]  = m32;
        m[6]  = m13;
        m[7]  = m23;
        m[8]  = m33;
    }

    void Matrix3::set(const float* array)
    {
        assert(array);
        std::copy(array, array + sizeof(m) / sizeof(float), m);
    }

    void Matrix3::setIdentity()
    {
        std::copy(IDENTITY.m, IDENTITY.m + sizeof(IDENTITY.m) / sizeof(float), m);
    }

    void Matrix3::setZero()
    {
        std::fill(m, m + sizeof(m) / sizeof(float), 0.0f);
    }

    void Matrix3::subtract(const Matrix3& matrix)
    {
        subtract(*this, matrix, *this);
    }

    void Matrix3::subtract(const Matrix3& m1, const Matrix3& m2, Matrix3& dst)
    {
        dst.m[0]  = m1.m[0]  - m2.m[0];
        dst.m[1]  = m1.m[1]  - m2.m[1];
        dst.m[2]  = m1.m[2]  - m2.m[2];
        dst.m[3]  = m1.m[3]  - m2.m[3];
        dst.m[4]  = m1.m[4]  - m2.m[4];
        dst.m[5]  = m1.m[5]  - m2.m[5];
        dst.m[6]  = m1.m[6]  - m2.m[6];
        dst.m[7]  = m1.m[7]  - m2.m[7];
        dst.m[8]  = m1.m[8]  - m2.m[8];
    }

    void Matrix3::transformVector(const Vector3& vector, Vector3& dst) const
    {
        // Handle case where v == dst.
        dst.v[0] = vector.v[0] * m[0] + vector.v[1] * m[3] + vector.v[2] * m[6];
        dst.v[1] = vector.v[0] * m[1] + vector.v[1] * m[4] + vector.v[2] * m[7];
        dst.v[2] = vector.v[0] * m[2] + vector.v[1] * m[5] + vector.v[2] * m[8];
    }

    void Matrix3::translate(float x, float y)
    {
        translate(x, y, *this);
    }

    void Matrix3::translate(float x, float y, Matrix3& dst) const
    {
        Matrix3 t;
        createTranslation(x, y, t);
        multiply(*this, t, dst);
    }

    void Matrix3::translate(const Vector2& t)
    {
        translate(t.v[0], t.v[1], *this);
    }

    void Matrix3::translate(const Vector2& t, Matrix3& dst) const
    {
        translate(t.v[0], t.v[1], dst);
    }

    void Matrix3::transpose()
    {
        transpose(*this);
    }

    void Matrix3::transpose(Matrix3& dst) const
    {
        float t[9] = {
            m[0], m[3], m[6],
            m[1], m[4], m[7],
            m[2], m[5], m[8]
        };
        std::copy(t, t + sizeof(t) / sizeof(float), dst.m);
    }
}
