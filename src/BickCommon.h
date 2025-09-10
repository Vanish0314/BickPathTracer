/*
 * @Author: Vanish
 * @Date: 2025-09-10 22:00:00
 * @LastEditTime: 2025-09-10 22:00:00
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#pragma once

// 通用数学常量
namespace BickConstants {
    constexpr double PI = 3.1415926535897932;
}

// 标准库包含
#include <cmath>
#include <random>
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

// 前向声明
class Scene;
class Ray;
class Material;
class HitRecord;
class Interval;
class Hittable;
class Vector3;

// 全局变量（为了向后兼容，将来应该移除）
extern Scene* g_Scene;

// 坐标系说明：左手坐标系,y轴向上,z轴向外,x轴向右