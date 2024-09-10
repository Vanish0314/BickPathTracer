/*
 * @Author: Vanish
 * @Date: 2024-09-05 19:37:37
 * @LastEditTime: 2024-09-05 20:12:24
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#pragma once

#include "Hittable.h"
#include "Vector3.h"
#include <cfloat>


struct Bounds
{
public:
    Vector3 min;
    Vector3 max;

    Bounds():min(Vector3(FLT_MAX, FLT_MAX, FLT_MAX)), max(Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX)) {}
    Bounds(const Vector3& min, const Vector3& max) : min(min), max(max) {}
    ~Bounds() {}

    void Expand(const Vector3& point)
    {
        min = Vector3::Min(min, point);
        max = Vector3::Max(max, point);
    }

    bool Intersect(const Ray &ray, double t_min, double t_max) const;
    
    Vector3 GetDiagonal() const
    {
        return max - min;
    }
};