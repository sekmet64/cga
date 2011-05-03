#pragma once

#include <cmath>
#include <GL/glew.h>
#include <iostream>

namespace cg
{
    //-------------------
    // class DCoordinate2
    //-------------------
    class DCoordinate2
    {
    protected:
        GLdouble _data[2];

    public:

        // default constructor
        DCoordinate2();

        // special constructor
        DCoordinate2(GLdouble x, GLdouble y);

        // get components by value
        const GLdouble operator [](GLuint index) const;
        const GLdouble x() const;
        const GLdouble y() const;


        // get components by reference
        GLdouble& operator [](GLuint index);
        GLdouble& x();
        GLdouble& y();

        // change sign
        const DCoordinate2 operator +() const;
        const DCoordinate2 operator -() const;

        // add
        const DCoordinate2 operator +(const DCoordinate2& rhs) const;

        // add to *this
        DCoordinate2& operator +=(const DCoordinate2& rhs);

        // subtract
        const DCoordinate2 operator -(const DCoordinate2& rhs) const;

        // subtract from *this
        DCoordinate2& operator -=(const DCoordinate2& rhs);

        // cross product
        const GLdouble operator ^(const DCoordinate2& rhs) const;

        // dot product
        const GLdouble operator *(const DCoordinate2& rhs) const;

        // scale
        const DCoordinate2 operator *(const GLdouble& rhs) const;
        const DCoordinate2 operator /(const GLdouble& rhs) const;

        // scale *this
        DCoordinate2& operator *=(const GLdouble& rhs);
        DCoordinate2& operator /=(const GLdouble& rhs);

        // length
        const GLdouble length() const;

        // normalize
        DCoordinate2& normalize();
    };

    //-------------------------------------
    // implementation of class DCoordinate2
    //-------------------------------------

    // default constructor
    inline DCoordinate2::DCoordinate2()
    {
        _data[0] = _data[1] = 0.0;
    }

    // special constructor
    inline DCoordinate2::DCoordinate2(GLdouble x, GLdouble y)
    {
        _data[0] = x;
        _data[1] = y;
    }

    // get components by value
    inline const GLdouble DCoordinate2::operator [](GLuint index) const
    {
        return _data[index];
    }

    inline const GLdouble DCoordinate2::x() const
    {
        return _data[0];
    }

    inline const GLdouble DCoordinate2::y() const
    {
        return _data[1];
    }

    // get components by reference
    inline GLdouble& DCoordinate2::operator [](GLuint index)
    {
        return _data[index];
    }

    inline GLdouble& DCoordinate2::x()
    {
        return _data[0];
    }

    inline GLdouble& DCoordinate2::y()
    {
        return _data[1];
    }

    // change sign
    inline const DCoordinate2 DCoordinate2::operator +() const
    {
        return DCoordinate2(_data[0], _data[1]);
    }

    inline const DCoordinate2 DCoordinate2::operator -() const
    {
        return DCoordinate2(-_data[0], -_data[1]);
    }

    // add
    inline const DCoordinate2 DCoordinate2::operator +(const DCoordinate2& rhs) const
    {
        return DCoordinate2(_data[0] + rhs._data[0], _data[1] + rhs._data[1]);
    }

    // add to *this
    inline DCoordinate2& DCoordinate2::operator +=(const DCoordinate2& rhs)
    {
        _data[0] += rhs._data[0];
        _data[1] += rhs._data[1];

        return *this;
    }

    // subtract
    inline const DCoordinate2 DCoordinate2::operator -(const DCoordinate2& rhs) const
    {
        return DCoordinate2(_data[0] - rhs._data[0], _data[1] - rhs._data[1]);
    }

    // subtract from *this
    inline DCoordinate2& DCoordinate2::operator -=(const DCoordinate2& rhs)
    {
        _data[0] -= rhs._data[0];
        _data[1] -= rhs._data[1];

        return *this;
    }

    // cross product
    inline const GLdouble DCoordinate2::operator ^(const DCoordinate2& rhs) const
    {
        return _data[0] * rhs._data[1] - _data[1] * rhs._data[0];
    }

    // dot product
    inline const GLdouble DCoordinate2::operator *(const DCoordinate2& rhs) const
    {
        return _data[0] * rhs._data[0] + _data[1] * rhs._data[1];
    }

    // scale
    inline const DCoordinate2 DCoordinate2::operator *(const GLdouble& rhs) const
    {
        return DCoordinate2(_data[0] * rhs, _data[1] * rhs);
    }

    inline const DCoordinate2 operator *(const GLdouble& lhs, const DCoordinate2& rhs)
    {
        return DCoordinate2(lhs * rhs[0], lhs * rhs[1]);
    }

    inline const DCoordinate2 DCoordinate2::operator /(const GLdouble& rhs) const
    {
        return DCoordinate2(_data[0] / rhs, _data[1] / rhs);
    }

    // scale *this
    inline DCoordinate2& DCoordinate2::operator *=(const GLdouble& rhs)
    {
        _data[0] *= rhs;
        _data[1] *= rhs;

        return *this;
    }

    inline DCoordinate2& DCoordinate2::operator /=(const GLdouble& rhs)
    {
        _data[0] /= rhs;
        _data[1] /= rhs;

        return *this;
    }

    // length
    inline const GLdouble DCoordinate2::length() const
    {
        return std::sqrt((*this) * (*this));
    }

    // normalize
    inline DCoordinate2& DCoordinate2::normalize()
    {
        GLdouble l = length();

        if (l && l != 1.0)
            *this /= l;

        return *this;
    }


    //----------------------------------------------------------------
    // definitions of overloaded input/output from/to stream operators
    //----------------------------------------------------------------

    // output to stream
    inline std::ostream& operator <<(std::ostream& lhs, const DCoordinate2& rhs)
    {
        return lhs << rhs[0] << " " << rhs[1];
    }

    // input from stream
    inline std::istream& operator >>(std::istream& lhs, DCoordinate2& rhs)
    {
        return lhs >> rhs[0] >> rhs[1];
    }
}
