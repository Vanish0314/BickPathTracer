/*
 * @Author: Vanish
 * @Date: 2024-07-14 01:12:58
 * @LastEditTime: 2024-07-14 01:17:39
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#pragma once

#include "Bick.h"
#include "Scene.h"
#include "Vector3.h"
#include "Ray.h"

class Debuger
{
public:
    
static bool DebugMode();
static Vector3 DebugResult(Ray& ray);

};