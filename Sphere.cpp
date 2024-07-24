/*
 * @Author: Vanish
 * @Date: 2024-05-31 22:47:58
 * @LastEditTime: 2024-06-12 18:45:26
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#include "Sphere.h"

double Sphere::GetUAt(const Vector3& hitPoint) const {
    // 获取球面坐标系下hitPoint的u坐标
    double theta = std::acos(hitPoint.z / radius);
    double phi = std::atan2(hitPoint.y, hitPoint.x);
    return (phi + 3.1415926535897932) / (2 * 3.1415926535897932);
}

double Sphere::GetVAt(const Vector3& hitPoint) const {
    // 获取球面坐标系下hitPoint的v坐标
    double theta = std::acos(hitPoint.y / radius);
    double phi = std::atan2(hitPoint.z, hitPoint.x);
    return (theta + 3.1415926535897932) / (2 * 3.1415926535897932);
}