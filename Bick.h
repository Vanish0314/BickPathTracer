/*
 * @Author: Vanish
 * @Date: 2024-05-31 03:56:56
 * @LastEditTime: 2024-07-14 15:30:55
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#pragma once

/*****************************************
 *                                       *
 *               控制参数                 *
 *                                       *
 *****************************************/

#define PI 3.1415926535897932
#define SAMPLE_COUNT 16
#define RUSSIAN_ROULETTE_PROBABILITY 0.7
#define DEBUG_LOG_GAP 1000

/*****************************************
 *                                       *
 *               切换场景                 *
 *                                       *
 *****************************************/

//#define SCENE_TEXTURE// 纹理场景
//#define SCENE_CONELLBOX // 康奈尔盒子
//#define SCENE_PBR// PBR 场景
#define SCENE_MESH// Mesh 场景

/*****************************************
 *                                       *
 *               控制台输出                *
 *                                       *
 *****************************************/

//#define DEBUG_TRACERAY
#define CONSOLE_OUTPUT_RENDER_PROGRESS

/*****************************************
 *                                       *
 *               DEBUG                   *
 *                                       *
 *****************************************/
//#define RENDER_DEBUG_NORMAL
//#define RENDER_DEBUG_UV
//#define RENDER_DEBUG_HITPOINT
//#define RENDER_DEBUG_DEPTH
//#define RENDER_DEBUG_RAYGEN
//#define RENDER_DEBUG_TRACE_RAY
//#define RENDER_DEBUG_SAMPLE
//#define RENDER_DEBUG_SHOW_ALBEDO
//#define RENDER_DEBUG_SHOW_ROUGHNESS
//#define RENDER_DEBUG_SHOW_METALLIC







#include <cmath>
#include <random>
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

// #include "Vector3.h"
// #include "Camera.h"
// #include "Sphere.h
// #include "Quad.h"
// #include "Light.h"
// #include "Material.h"

// 前向声明
class Scene;
class Ray;
class Material;
class HitRecord;
class Interval;
class Hittable;


// 定义全局变量
extern Scene* g_Scene;

//左手坐标系,y轴向上,z轴向外,x轴向右