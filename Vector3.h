/*
 * @Author: Vanish
 * @Date: 2024-05-31 03:57:21
 * @LastEditTime: 2024-09-08 04:57:10
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#pragma once

#include <cmath>
#include <algorithm>

#include "Random.h"

class Vector3
{
    
public:
    double x =0, y = 0, z = 0;
public:
    Vector3(double x = 0, double y = 0, double z = 0): x(x), y(y), z(z){}
    ~Vector3(){};
public:
    Vector3 operator+(const Vector3& other) const
    {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }
    Vector3 operator+(const double& scalar) const
    {
        return Vector3(x + scalar, y + scalar, z + scalar);
    }
    Vector3 operator+=(const Vector3& other) 
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    bool operator== (const Vector3& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }
    Vector3 operator-(const Vector3& other) const
    {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }
    Vector3 operator*(const double& scalar) const
    {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }
    friend Vector3 operator*(const double& scalar, const Vector3& vec)
    {
        return Vector3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
    }
    Vector3 operator*(const Vector3& other) const
    {
        return Vector3(x * other.x, y * other.y, z * other.z);
    }
    Vector3 operator/(const double& scalar) const
    {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }
    Vector3 operator/(const Vector3& scalar)
    {
        x /= scalar.x;
        y /= scalar.y;
        z /= scalar.z;
        return *this;
    }
    double operator[](int index) const
    {
        switch (index)
        {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        }
    }
    double Magnitude() const
    {
        return sqrt(x*x + y*y + z*z);
    }
    void Normalize()
    {
        double mag = Magnitude();
        if (mag > 0)
        {
            x /= mag;
            y /= mag;
            z /= mag;
        }
    }
    Vector3 Normalized() const
    {
        Vector3 v = *this;
        v.Normalize();
        return v;
    }

    double Dot(const Vector3& other) const
    {
        return x*other.x + y*other.y + z*other.z;
    }

    Vector3 Cross(const Vector3& other) const
    {
        //右手坐标系下叉乘
        return Vector3(
            y*other.z - z*other.y,
            z*other.x - x*other.z,
            x*other.y - y*other.x
        );

        //左手坐标系下叉乘
        return Vector3(
           y*other.z - z*other.y,
           x*other.z - z*other.x,
           x*other.y - y*other.x
        );
    }
public:
    static double Dot(const Vector3& a, const Vector3& b)
    {
        return a.x*b.x + a.y*b.y + a.z*b.z;
    }

    static Vector3 Cross(const Vector3& a, const Vector3& b)
    {
        return Vector3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
    }

    static Vector3 Lerp(const Vector3& a, const Vector3& b, double t)
    {
        return a + (b - a) * t;
    }
    

public:
    static double Distance(const Vector3& a, const Vector3& b)
    {
        double dx = a.x - b.x;
        double dy = a.y - b.y;
        double dz = a.z - b.z;
        return sqrt(dx*dx + dy*dy + dz*dz);
    }

    static double Angle(const Vector3& a, const Vector3& b)
    {
        double dot = a.Dot(b);
        double magA = a.Magnitude();
        double magB = b.Magnitude();
        if (magA > 0 && magB > 0)
        {
            return acos(dot / (magA * magB));
        }
        return 0;
    }


    static Vector3 Reflect(const Vector3& incident, const Vector3& normal)
    {
        return incident - normal * 2 * normal.Dot(incident);
    }

    static Vector3 Refract(const Vector3& incident, const Vector3& normal, double eta)
    {
        double dot = normal.Dot(incident);
        double k = 1 - eta * eta * (1 - dot * dot);
        if (k < 0)
        {
            return Vector3(0, 0, 0);
        }
        return  incident * eta + normal * (eta * dot - sqrt(k));
    }

    static Vector3 Random()
    {
        return Vector3(
            Random::GetRandomDouble(-1,1),
            Random::GetRandomDouble(-1,1),
            Random::GetRandomDouble(-1,1)
        ).Normalized();
    }

    static Vector3 Random(double min, double max)
    {
        return Vector3
            (Random::GetRandomDouble(min,max),
             Random::GetRandomDouble(min,max),
             Random::GetRandomDouble(min,max)
            ).Normalized();
    }


    static Vector3 RandomUnit()
    {
        Vector3 v = Random();
        v.Normalize();
        return v;
    }

    static Vector3 RandomInUnitDisk()
    {
        Vector3 v = Vector3(rand() / (double)RAND_MAX, rand() / (double)RAND_MAX, 0);
        while (v.Magnitude() >= 1)
        {
            v = Vector3(rand() / (double)RAND_MAX, rand() / (double)RAND_MAX, 0);
        }
        return v;
    }


    static Vector3 RandomInHemisphere(const Vector3& normal)
    {
        Vector3 v = RandomUnit();
        if (v.Dot(normal) < 0)
        {
            v = Vector3(0, 0, 0) - v;
        }
        return v;
    }

    static Vector3 Min(const Vector3& a, const Vector3& b)
    {
        return Vector3(
            std::min(a.x, b.x),
            std::min(a.y, b.y),
            std::min(a.z, b.z)
        );
    }

    static Vector3 Max(const Vector3& a, const Vector3& b)
    {
        return Vector3(
            std::max(a.x, b.x),
            std::max(a.y, b.y),
            std::max(a.z, b.z)
        );
    }

};

