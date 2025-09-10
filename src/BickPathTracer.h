/*
 * @Author: Vanish
 * @Date: 2025-09-10 21:00:00
 * @LastEditTime: 2025-09-10 21:00:00
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#pragma once

// 数学常量

// 标准库头文件
#include <cmath>
#include <random>
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

// 数学库
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Color.h"
#include "math/Random.h"
#include "math/Bounds.h"

// 核心组件
#include "core/Ray.h"
#include "core/Scene.h"
#include "core/Camera.h"
#include "core/Film.h"

// 几何体
#include "geometry/Hittable.h"
#include "geometry/Sphere.h"
#include "geometry/Quad.h"
#include "geometry/Box.h"
#include "geometry/Triangle.h"
#include "geometry/Mesh.h"

// 材质系统
#include "material/Material.h"
#include "material/Texture.h"

// 加速结构
#include "acceleration/BVH.h"

// 工具类
#include "utils/ThreadPool.h"
#include "utils/Profiler.h"
#include "utils/Debuger.h"
#include "utils/SpinLock.h"

// 配置系统
#include "config/RenderConfig.h"
#include "config/SceneFactory.h"

// 前向声明
class Vector3;
class Film;
class HitRecord;
class Interval;
class Ray;
class PDF;
class SampleResult;

// 渲染器主类
class Renderer {
public:
    Renderer(const RenderConfig& config);
    ~Renderer();
    
    bool Initialize();
    bool Render();
    void Cleanup();
    
private:
    RenderConfig m_config;
    std::unique_ptr<Scene> m_scene;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<ThreadPool> m_threadPool;
    
    // 渲染核心函数
    Vector3 Shade(const Vector3& p, const Vector3& wo, const Vector3& normal, 
                  double t, std::shared_ptr<Material_PBM> material);
    Vector3 ShadeWithDebug(const Vector3& p, const Vector3& wo, const Vector3& normal, 
                          double t, std::shared_ptr<Material_PBM> material, 
                          Film& debugFilm, int x, int y, int depth);
    
    // 新的路径追踪实现
    Vector3 PathTrace(Ray ray, int depth);
    Vector3 SampleDirectLight(const Vector3& hitPoint, const class LocalFrame& frame, 
                             const Vector3& view_direction, class SimpleDiffuseMaterial* material);
    class SimpleDiffuseMaterial* GetSimpleMaterial(std::shared_ptr<Material> material);
    
    // 调试渲染
    void RenderDebugImages();
    
    // 进度输出
    void ShowProgress(int currentSamples, int totalSamples);
};
