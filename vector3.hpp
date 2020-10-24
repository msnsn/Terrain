
#pragma once

class Vector3
{
public:
    Vector3(float _x, float _y, float _z) :
        x(_x),y(_y),z(_z)
    {

    }

    Vector3() :
        x(0),y(0),z(0)
    {

    }

    ~Vector3()
    {

    }

    float x, y, z;

    bool operator == (Vector3 const & vec)
    {
        return (vec.x == x && vec.y == y && vec.z == z);
    }

    bool operator != (Vector3 const & vec)
    {
        return (vec.x != x || vec.y != y || vec.z != z);
    }

    Vector3 operator + (Vector3 const & vec)
    {
        return (Vector3(x+vec.x, y+vec.y, z+vec.z));
    }

    Vector3 operator - (Vector3 const & vec)
    {
        return (Vector3(x-vec.x, y-vec.y, z-vec.z));
    }

    Vector3 operator * (Vector3 const & vec)
    {
        return (Vector3(x*vec.x, y*vec.y, z*vec.z));
    }

    Vector3 operator *= (Vector3 const & vec)
    {
        return (Vector3(x*vec.x, y*vec.y, z*vec.z));
    }

    Vector3 operator / (Vector3 const & vec)
    {
        return (Vector3(x/vec.x, y/vec.y, z/vec.z));
    }


    Vector3 operator + (float const & fl)
    {
        return (Vector3(x+fl, y+fl, z+fl));
    }

    Vector3 operator - (float const & fl)
    {
        return (Vector3(x-fl, y-fl, z-fl));
    }

    Vector3 operator * (float const & fl)
    {
        return (Vector3(x*fl, y*fl, z*fl));
    }

    Vector3 operator *= (float const & fl)
    {
        return (Vector3(x*fl, y*fl, z*fl));
    }

    Vector3 operator / (float const & fl)
    {
        return (Vector3(x/fl, y/fl, z/fl));
    }

};

