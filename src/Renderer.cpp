/*
 * @Author: Vanish
 * @Date: 2025-09-10 21:10:00
 * @LastEditTime: 2025-09-10 21:10:00
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */

#include "BickPathTracer.h"
#include "material/SimpleMaterial.h"
#include <thread>
#include <iomanip>
#include <algorithm>
#include <map>

// 定义全局变量（为了兼容旧代码）
Scene* g_Scene = nullptr;

Renderer::Renderer(const RenderConfig& config) : m_config(config) {
}

Renderer::~Renderer() {
    Cleanup();
}

bool Renderer::Initialize() {
    // 创建场景
    if (m_config.sceneConfigPath.empty() || m_config.sceneConfigPath == "pbr_demo") {
        m_scene = SceneFactory::CreateScene(SceneType::PBR_DEMO);
    } else if (m_config.sceneConfigPath == "cornell_box") {
        m_scene = SceneFactory::CreateScene(SceneType::CORNELL_BOX);
    } else if (m_config.sceneConfigPath == "texture_demo") {
        m_scene = SceneFactory::CreateScene(SceneType::TEXTURE_DEMO);
    } else if (m_config.sceneConfigPath == "mesh_demo") {
        m_scene = SceneFactory::CreateScene(SceneType::MESH_DEMO);
    } else {
        // 从文件加载场景
        m_scene = SceneFactory::CreateSceneFromFile(m_config.sceneConfigPath);
    }
    
    if (!m_scene) {
        std::cerr << "场景创建失败!" << std::endl;
        return false;
    }
    
    // 设置全局场景变量（为了兼容旧代码）
    g_Scene = m_scene.get();
    
    // 创建相机
    m_camera = std::make_unique<Camera>(
        Vector3(m_config.camera.position[0], m_config.camera.position[1], m_config.camera.position[2]),
        Vector3(m_config.camera.direction[0], m_config.camera.direction[1], m_config.camera.direction[2]),
        Vector3(m_config.camera.up[0], m_config.camera.up[1], m_config.camera.up[2]),
        m_config.render.imageWidth,
        m_config.render.imageHeight,
        m_config.camera.fov
    );
    
    // 创建线程池
    int threadCount = m_config.render.threadCount;
    if (threadCount == 0) {
        threadCount = std::thread::hardware_concurrency();
        if (threadCount == 0) threadCount = 4; // 备用值
    }
    m_threadPool = std::make_unique<ThreadPool>();
    
    std::cout << "场景加载完成" << std::endl;
    std::cout << "使用线程数: " << threadCount << std::endl;
    
    return true;
}

bool Renderer::Render() {
    if (!m_scene || !m_camera) {
        std::cerr << "渲染器未正确初始化!" << std::endl;
        return false;
    }
    
    // 如果启用了调试模式，先渲染调试图像
    if (m_config.render.renderDebugImages) {
        RenderDebugImages();
    }
    
    // 主渲染
    Film film(m_config.render.imageWidth, m_config.render.imageHeight);
    int currentSamples = 0;
    int increaseStep = std::min(128, m_config.render.sampleCount / 10);
    if (increaseStep <= 0) increaseStep = 1;
    
    // 计算最大光照强度用于clamp
    Vector3 maxLi = Vector3(0, 0, 0);
    for (auto light : m_scene->lights) {
        auto material = std::dynamic_pointer_cast<Material_PBM>(light->material);
        if (material && material->isEmissive) {
            maxLi += material->emissiveDistribution * material->emissiveIntensity;
        }
    }
    
    while (currentSamples < m_config.render.sampleCount) {
        m_threadPool->ParallelFor(m_config.render.imageWidth, m_config.render.imageHeight, 
            [&](int i, int j) {
                for (int s = 0; s < increaseStep; s++) {
                    if (currentSamples + s >= m_config.render.sampleCount) break;
                    
                    Vector3 radiance = Vector3(0, 0, 0);
                    Ray ray = Ray(Vector3(0, 0, 0), Vector3(0, 0, 0));
                    m_camera->GetRay(i, j, ray);
                    
                    HitRecord hitRecord;
                    m_scene->Hit(ray, Interval(1e-4, 1e10), hitRecord);
                    
                    // 直接使用新的路径追踪算法
                    radiance = PathTrace(ray, 0);
                    
                    // 颜色clamp，避免过亮或负值
                    radiance = Vector3(
                        std::max(0.0, std::min(radiance.x, 10.0)),
                        std::max(0.0, std::min(radiance.y, 10.0)),
                        std::max(0.0, std::min(radiance.z, 10.0))
                    );
                    
                    film.AddSample(i, j, radiance);
                }
            });
        
        currentSamples += increaseStep;
        m_threadPool->WaitAll();
        
        // 保存中间结果
        film.Save_P3(m_config.render.outputPath);
        
        // 显示进度
        if (m_config.render.showRenderProgress) {
            ShowProgress(currentSamples, m_config.render.sampleCount);
        }
    }
    
    // 保存最终结果
    film.Save_P3(m_config.render.outputPath);
    std::cout << "渲染结果已保存到: " << m_config.render.outputPath << std::endl;
    
    return true;
}

void Renderer::Cleanup() {
    m_scene.reset();
    m_camera.reset();
    m_threadPool.reset();
}

// 新的路径追踪渲染函数，基于参考项目的正确实现
Vector3 Renderer::PathTrace(Ray ray, int depth) {
    const int MAX_DEPTH = 10;
    Vector3 beta(1, 1, 1);  // 路径权重
    Vector3 L(0, 0, 0);     // 累积辐射度
    bool last_is_specular = true;
    
    for (int bounces = 0; bounces < MAX_DEPTH; bounces++) {
        HitRecord hitRecord;
        m_scene->Hit(ray, Interval(1e-4, 1e10), hitRecord);
        
        if (!hitRecord.hitted) {
            break; // 没有击中任何物体
        }
        
        // 转换为简化材质
        SimpleDiffuseMaterial* simpleMat = GetSimpleMaterial(hitRecord.material);
        if (!simpleMat) break;
        
        // 如果击中光源，累积发光
        if (simpleMat->isEmissive) {
            L = L + beta * simpleMat->getEmission();
            break;
        }
        
        // 俄罗斯轮盘赌终止
        if (bounces > 3) {
            double q = std::max(0.05, 1.0 - std::max({beta.x, beta.y, beta.z}));
            if (Random::GetRandomDouble(0, 1) < q) {
                break;
            }
            beta = beta * (1.0 / (1.0 - q));
        }
        
        // 建立局部坐标系
        LocalFrame frame(hitRecord.normal);
        Vector3 view_direction = frame.localFromWorld(Vector3(0,0,0) - ray.direction);
        
        if (abs(view_direction.y) < 1e-6) {
            break;
        }
        
        // 直接光照采样（对于非镜面材质）
        if (!simpleMat->isDeltaDistribution()) {
            Vector3 directLight = SampleDirectLight(hitRecord.hitPoint, frame, view_direction, simpleMat);
            L = L + beta * directLight;
        }
        
        // BSDF采样获得下一个方向
        auto bsdf_sample = simpleMat->sampleBSDF(hitRecord.hitPoint, view_direction);
        if (!bsdf_sample.has_value()) {
            break;
        }
        
        // 更新路径权重
        beta = beta * bsdf_sample->bsdf * abs(bsdf_sample->light_direction.y) / bsdf_sample->pdf;
        
        // 设置下一条射线
        ray.origin = hitRecord.hitPoint;
        ray.direction = frame.worldFromLocal(bsdf_sample->light_direction);
        last_is_specular = simpleMat->isDeltaDistribution();
    }
    
    return L;
}

// 直接光照采样
Vector3 Renderer::SampleDirectLight(const Vector3& hitPoint, const LocalFrame& frame, 
                                   const Vector3& view_direction, SimpleDiffuseMaterial* material) {
    Vector3 directLight(0, 0, 0);
    
    // 对每个光源采样
    for (auto light : m_scene->lights) {
        SampleResult sample = light->UnitSamplePdf();
        Vector3 lightDir = (sample.position - hitPoint).Normalized();
        double lightDistance = Vector3::Distance(hitPoint, sample.position);
        
        // 阴影射线测试
        Ray shadowRay(hitPoint + frame.y_axis * 1e-4, lightDir);
        HitRecord shadowHit;
        m_scene->Hit(shadowRay, Interval(1e-4, lightDistance - 1e-4), shadowHit);
        
        if (!shadowHit.hitted) {
            // 转换到局部坐标系
            Vector3 light_direction_local = frame.localFromWorld(lightDir);
            
            if (light_direction_local.y > 0) { // 确保在正确半球
                // 获取光源发光
                auto lightMaterial = std::dynamic_pointer_cast<Material_PBM>(light->material);
                Vector3 Le = lightMaterial ? lightMaterial->EmissiveTerm(Vector3(0,0,0) - lightDir, sample.position) : Vector3(1,1,1);
                
                // BSDF值
                Vector3 f = material->BSDF(hitPoint, light_direction_local, view_direction);
                
                // 几何项
                double cosTheta = abs(light_direction_local.y);
                double cosLightTheta = std::max(0.0, Vector3::Dot(Vector3(0,0,0) - lightDir, sample.normal));
                
                // 立体角
                double solidAngle = (cosLightTheta * sample.pdf) / (lightDistance * lightDistance + 1e-6);
                
                directLight = directLight + Le * f * cosTheta * solidAngle;
            }
        }
    }
    
    return directLight;
}

// 获取简化材质的辅助函数
SimpleDiffuseMaterial* Renderer::GetSimpleMaterial(std::shared_ptr<Material> material) {
    auto pbmMat = std::dynamic_pointer_cast<Material_PBM>(material);
    if (!pbmMat) return nullptr;
    
    // 创建静态材质映射以避免重复创建
    static std::map<Material*, std::unique_ptr<SimpleDiffuseMaterial>> materialMap;
    
    if (materialMap.find(material.get()) == materialMap.end()) {
        if (pbmMat->isEmissive) {
            materialMap[material.get()] = std::make_unique<SimpleDiffuseMaterial>(
                pbmMat->emissiveDistribution * pbmMat->emissiveIntensity, true);
        } else {
            // 使用当前的albedo缓冲区
            Vector3 albedo(pbmMat->albedoBuffer.r, pbmMat->albedoBuffer.g, pbmMat->albedoBuffer.b);
            materialMap[material.get()] = std::make_unique<SimpleDiffuseMaterial>(albedo);
        }
    }
    
    return materialMap[material.get()].get();
}

Vector3 Renderer::Shade(const Vector3& p, const Vector3& wo, const Vector3& normal, 
                        double t, std::shared_ptr<Material_PBM> material) {
    // 保持旧接口兼容性，内部调用新的路径追踪
    Ray ray(p, wo);
    return PathTrace(ray, 0);
}

Vector3 Renderer::ShadeWithDebug(const Vector3& p, const Vector3& wo, const Vector3& normal, 
                                 double t, std::shared_ptr<Material_PBM> material, 
                                 Film& debugFilm, int x, int y, int depth) {
    // 调试版本的Shade函数，添加了调试信息记录
    // 实现与Shade类似，但会记录调试信息到debugFilm
    return Shade(p, wo, normal, t, material);
}

void Renderer::RenderDebugImages() {
    std::cout << "开始渲染调试图像..." << std::endl;
    
    // 创建各种调试用的Film
    Film normal(m_config.render.imageWidth, m_config.render.imageHeight);
    Film uv(m_config.render.imageWidth, m_config.render.imageHeight);
    Film depth(m_config.render.imageWidth, m_config.render.imageHeight);
    Film albedo(m_config.render.imageWidth, m_config.render.imageHeight);
    
    m_threadPool->ParallelFor(m_config.render.imageWidth, m_config.render.imageHeight, 
        [&](int i, int j) {
            Ray ray = Ray(Vector3(0, 0, 0), Vector3(0, 0, 0));
            m_camera->GetRay(i, j, ray);
            HitRecord hitRecord;
            m_scene->Hit(ray, Interval(), hitRecord);
            
            if (hitRecord.hitted) {
                // 法线
                normal.AddSample(i, j, Vector3(
                    (hitRecord.normal.x + 1) / 2,
                    (hitRecord.normal.y + 1) / 2,
                    (hitRecord.normal.z + 1) / 2
                ) * 255);
                
                // UV坐标
                uv.AddSample(i, j, Vector3(hitRecord.u, hitRecord.v, 0) * 255);
                
                // 深度
                depth.AddSample(i, j, Vector3(1, 1, 1) * hitRecord.t * 12);
                
                // 反照率
                auto material = std::dynamic_pointer_cast<Material_PBM>(hitRecord.material);
                if (material) {
                    material->UpdateBuffer(hitRecord.u, hitRecord.v);
                    Color color = material->albedoBuffer;
                    albedo.AddSample(i, j, Vector3(color.r, color.g, color.b) * 255);
                }
            }
        });
    
    m_threadPool->WaitAll();
    
    // 保存调试图像
    std::string debugDir = m_config.render.debugOutputDir;
    normal.Save_P3_WithoutCorrection(debugDir + "normal.ppm");
    uv.Save_P3_WithoutCorrection(debugDir + "uv.ppm");
    depth.Save_P3_WithoutCorrection(debugDir + "depth.ppm");
    albedo.Save_P3_WithoutCorrection(debugDir + "albedo.ppm");
    
    std::cout << "调试图像保存完成" << std::endl;
}

void Renderer::ShowProgress(int currentSamples, int totalSamples) {
    double progress = (double)currentSamples / totalSamples * 100.0;
    std::cout << "渲染进度: " << currentSamples << "/" << totalSamples 
              << " (" << std::fixed << std::setprecision(1) << progress << "%)" << std::endl;
}
