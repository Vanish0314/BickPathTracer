/*
 * @Author: Vanish
 * @Date: 2025-09-10 21:10:00
 * @LastEditTime: 2025-09-10 21:10:00
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */

#include "BickPathTracer.h"
#include <thread>
#include <iomanip>

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
                    if (currentSamples >= m_config.render.sampleCount) break;
                    
                    Vector3 radiance = Vector3(0, 0, 0);
                    Ray ray = Ray(Vector3(0, 0, 0), Vector3(0, 0, 0));
                    m_camera->GetRay(i, j, ray);
                    
                    HitRecord hitRecord;
                    m_scene->Hit(ray, Interval(), hitRecord);
                    if (!hitRecord.hitted) break;
                    
                    auto material = std::dynamic_pointer_cast<Material_PBM>(hitRecord.material);
                    if (!material) break;
                    
                    radiance = Shade(hitRecord.hitPoint, Vector3(0, 0, 0) - ray.direction, 
                                   hitRecord.normal, hitRecord.t, material);
                    
                    // Clamp radiance
                    radiance = Vector3(
                        std::max(0.0, std::min(radiance.x, maxLi.x)),
                        std::max(0.0, std::min(radiance.y, maxLi.y)),
                        std::max(0.0, std::min(radiance.z, maxLi.z))
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

Vector3 Renderer::Shade(const Vector3& p, const Vector3& wo, const Vector3& normal, 
                        double t, std::shared_ptr<Material_PBM> material) {
    if (material->isEmissive) {
        return material->EmissiveTerm(wo, p) * Vector3::Dot(wo, normal) / m_config.render.russianRouletteProb;
    }
    
    Vector3 dirLight = Vector3(0, 0, 0);
    Vector3 indirLight = Vector3(0, 0, 0);
    
    // 重要性采样直接光照
    if (m_config.render.enableImportanceSampling) {
        for (auto light : m_scene->lights) {
            SampleResult sample = light->UnitSamplePdf();
            
            Ray occlusionRay(
                p + normal * 0.01,
                (sample.position - p).Normalized()
            );
            
            HitRecord hitRecord;
            m_scene->Hit(occlusionRay, Interval(0, Vector3::Distance(p, sample.position) - 0.01), hitRecord);
            
            if (!hitRecord.hitted) {
                auto lightMaterial = std::dynamic_pointer_cast<Material_PBM>(light->material);
                Vector3 emi = lightMaterial->EmissiveTerm(Vector3(0, 0, 0) - occlusionRay.direction, sample.position);
                Vector3 fr = material->BRDF(p, wo, occlusionRay.direction, normal);
                double cos1 = std::max(0.0, Vector3::Dot(occlusionRay.direction.Normalized(), normal));
                double delta = Vector3::Distance(p, sample.position);
                double term = cos1 / std::pow(delta, 2);
                double pdf = sample.pdf;
                
                dirLight = emi * fr * term / pdf;
            }
        }
    }
    
    // 俄罗斯轮盘
    if (Random::GetRandomDouble(0, 1) > m_config.render.russianRouletteProb) {
        return dirLight;
    }
    
    // 间接光照采样
    Ray ray(Vector3(0, 0, 0), Vector3(0, 0, 0));
    double pdf = PDF::SampleHemisphere(p, ray, normal);
    Vector3 wi = ray.direction;
    
    HitRecord bounceRecord;
    m_scene->Hit(ray, Interval(), bounceRecord);
    if (!bounceRecord.hitted) {
        return dirLight;
    }
    
    auto hitMaterial = std::dynamic_pointer_cast<Material_PBM>(bounceRecord.material);
    double cosTheta = std::max(0.0, Vector3::Dot(wi, normal));
    
    if (!hitMaterial->isEmissive) {
        Vector3 li = Shade(bounceRecord.hitPoint, Vector3(0, 0, 0) - wi, 
                          bounceRecord.normal, bounceRecord.t, hitMaterial);
        Vector3 fr = material->BRDF(p, wo, wi, normal, bounceRecord.u, bounceRecord.v);
        Vector3 result = li * fr * cosTheta / pdf / m_config.render.russianRouletteProb;
        indirLight = result;
    } else if (!m_config.render.enableImportanceSampling) {
        Vector3 emi = hitMaterial->EmissiveTerm(Vector3(0, 0, 0) - wi, p);
        Vector3 fr = material->BRDF(p, wo, wi, normal, bounceRecord.u, bounceRecord.v);
        Vector3 result = emi * fr * cosTheta / pdf / m_config.render.russianRouletteProb;
        return result;
    }
    
    return dirLight + indirLight;
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
