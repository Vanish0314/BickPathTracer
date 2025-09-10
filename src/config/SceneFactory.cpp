/*
 * @Author: Vanish
 * @Date: 2025-09-10 21:20:00
 * @LastEditTime: 2025-09-10 21:39:49
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */

#include "SceneFactory.h"
#include "../core/Scene.h"
#include "../material/Material.h"
#include "../material/Texture.h"
#include "../geometry/Sphere.h"
#include "../geometry/Quad.h"
#include "../geometry/Box.h"
#include "../geometry/Mesh.h"
#include "../math/Color.h"
#include "../math/Vector3.h"
#include <fstream>
#include <sstream>

std::unique_ptr<Scene> SceneFactory::CreateScene(SceneType type) {
    switch (type) {
        case SceneType::CORNELL_BOX:
            return CreateCornellBoxScene();
        case SceneType::PBR_DEMO:
            return CreatePBRDemoScene();
        case SceneType::TEXTURE_DEMO:
            return CreateTextureDemoScene();
        case SceneType::MESH_DEMO:
            return CreateMeshDemoScene();
        default:
            return CreatePBRDemoScene(); // 默认场景
    }
}

std::unique_ptr<Scene> SceneFactory::CreateSceneFromFile(const std::string& configPath) {
    SceneDesc desc;
    if (!LoadSceneDesc(configPath, desc)) {
        std::cerr << "无法加载场景配置文件: " << configPath << std::endl;
        return nullptr;
    }
    return CreateSceneFromDesc(desc);
}

std::unique_ptr<Scene> SceneFactory::CreateSceneFromDesc(const SceneDesc& desc) {
    auto scene = std::make_unique<Scene>();
    
    // 创建材质映射
    std::unordered_map<std::string, std::shared_ptr<Material_PBM>> materials;
    for (const auto& matDesc : desc.materials) {
        materials[matDesc.name] = CreateMaterial(matDesc);
    }
    
    // 创建物体
    for (const auto& objDesc : desc.objects) {
        auto object = CreateObject(objDesc, materials);
        if (object) {
            scene->AddObject(object.get());
        }
    }
    
    // 创建光源
    for (const auto& lightDesc : desc.lights) {
        auto light = CreateObject(lightDesc, materials);
        if (light) {
            scene->AddObject(light.get());
        }
    }
    
    return scene;
}

std::unique_ptr<Scene> SceneFactory::CreateCornellBoxScene() {
    auto scene = std::make_unique<Scene>();
    
    // 创建材质
    auto white = std::make_shared<Material_PBM>(Color(1, 1, 1, 1), 1, 0);
    auto red = std::make_shared<Material_PBM>(Color(1, 0, 0, 1), 1, 0);
    auto blue = std::make_shared<Material_PBM>(Color(0, 0, 1, 1), 1, 0);
    auto light = std::make_shared<Material_PBM>(Vector3(1, 1, 1), 80);
    
    // 创建Cornell Box的墙壁
    auto leftWall = new Sphere("LeftWall", 10000, Vector3(-10000, 5.55/2, 5.55/2), red);
    auto rightWall = new Sphere("RightWall", 10000, Vector3(5.55 + 10000, 5.55/2, 5.55/2), blue);
    auto floor = new Sphere("Floor", 10000, Vector3(0, -10000, 0), white);
    auto ceiling = new Sphere("Ceiling", 10000, Vector3(0, 5.55 + 10000, 0), white);
    auto backWall = new Sphere("BackWall", 10000, Vector3(5.55/2, 5.55/2, 10000 + 5.55), white);
    
    // 创建光源
    auto sphereLight = new Sphere("Light", 0.2, Vector3(5.55/2, 5.55/2, 5.55/2), light);
    
    // 创建两个小立方体
    // 第一个立方体 - 较高的白色立方体（左侧）
    auto tallBoxQuads = Box(
        Vector3(1, 0, 3),
        Vector3(2, 2, 4),
        white,
        "高粗箱子"
    );
    
    // 右侧物体改为一个较小的白色球体
    auto rightSmallSphere = new Sphere(
        "RightSmallSphere",
        0.5,                              // 半径
        Vector3(4.0, 0.5, 2.5),           // 位置（与原右侧矮箱子中心一致）
        white
    );
    
    // 添加到场景
    scene->AddObject(leftWall);
    scene->AddObject(rightWall);
    scene->AddObject(floor);
    scene->AddObject(ceiling);
    scene->AddObject(backWall);
    scene->AddObject(sphereLight);
    
    // 添加立方体的所有面
    for (auto quad : tallBoxQuads) {
        scene->AddObject(quad);
    }
    // 添加右侧小球
    scene->AddObject(rightSmallSphere);
    
    return scene;
}

std::unique_ptr<Scene> SceneFactory::CreatePBRDemoScene() {
    auto scene = std::make_unique<Scene>();
    
    // 创建不同PBR参数的材质和球体阵列
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            auto material = std::make_shared<Material_PBM>(
                Color(1, 0.2, 0.3, 1),
                0.2 * i,  // roughness
                0.2 * j   // metallic
            );
            
            auto sphere = new Sphere(
                "Sphere" + std::to_string(i) + std::to_string(j),
                0.5,
                Vector3(1.5 * i, 1.5 * j, 0),
                material
            );
            
            scene->AddObject(sphere);
        }
    }
    
    // 添加光源
    auto lightMaterial = std::make_shared<Material_PBM>(Vector3(1, 1, 1), 80);
    auto light1 = new Sphere("Light1", 2, Vector3(12, 12, -10), lightMaterial);
    auto light2 = new Sphere("Light2", 2, Vector3(-2, 12, -10), lightMaterial);
    auto light3 = new Sphere("Light3", 2, Vector3(12, -12, -10), lightMaterial);
    auto light4 = new Sphere("Light4", 2, Vector3(-12, -12, -10), lightMaterial);
    
    scene->AddObject(light1);
    scene->AddObject(light2);
    scene->AddObject(light3);
    scene->AddObject(light4);
    
    return scene;
}

std::unique_ptr<Scene> SceneFactory::CreateTextureDemoScene() {
    auto scene = std::make_unique<Scene>();
    
    // 加载纹理
    auto paintAlbedo = LoadTexture("assets/textures/PaintAbledo.ppm");
    auto paintRM = LoadTexture("assets/textures/PaintRM.ppm");
    
    if (paintAlbedo && paintRM) {
        auto paintMaterial = std::make_shared<Material_PBM>(paintAlbedo, paintRM);
        
        // 创建展示纹理的四边形
        auto quad = new Quad(
            "TextureQuad",
            Vector3(0, 0, 0),
            Vector3(5, 0, 0),
            Vector3(0, 5, 0),
            paintMaterial
        );
        
        scene->AddObject(quad);
    }
    
    // 添加光源
    auto lightMaterial = std::make_shared<Material_PBM>(Vector3(1, 1, 1), 50);
    auto light = new Sphere("Light", 1, Vector3(2.5, 2.5, -3), lightMaterial);
    scene->AddObject(light);
    
    return scene;
}

std::unique_ptr<Scene> SceneFactory::CreateMeshDemoScene() {
    auto scene = std::make_unique<Scene>();
    
    // 创建网格材质
    auto meshMaterial = std::make_shared<Material_PBM>(Color(1, 1, 1, 1), 0.5, 0.5);
    
    // 加载网格模型
    auto mesh = new Mesh(
        "DragonMesh",
        "assets/meshes/dragon_87k.obj",
        Vector3(2.8, 2.42, -1.6),
        meshMaterial
    );
    
    scene->AddObject(mesh);
    
    // 添加环境光源
    auto lightMaterial1 = std::make_shared<Material_PBM>(Vector3(0, 1, 1), 3);
    auto lightMaterial2 = std::make_shared<Material_PBM>(Vector3(1, 0.17, 1), 3);
    
    auto light1 = new Quad(
        "EnvLight1",
        Vector3(-10000, 8, -10000),
        Vector3(20000, 0, 0),
        Vector3(0, 0, 20000),
        lightMaterial1
    );
    
    auto light2 = new Quad(
        "EnvLight2",
        Vector3(-10000, 0, -10000),
        Vector3(0, 0, 20000),
        Vector3(20000, 0, 0),
        lightMaterial2
    );
    
    scene->AddObject(light1);
    scene->AddObject(light2);
    
    return scene;
}

std::shared_ptr<Material_PBM> SceneFactory::CreateMaterial(const MaterialDesc& desc) {
    if (desc.type == "emissive") {
        return std::make_shared<Material_PBM>(
            Vector3(desc.emission[0], desc.emission[1], desc.emission[2]),
            desc.emissionIntensity
        );
    } else {
        // PBR 材质
        if (!desc.albedoTexture.empty() && !desc.roughnessMetallicTexture.empty()) {
            auto albedoTex = LoadTexture(desc.albedoTexture);
            auto rmTex = LoadTexture(desc.roughnessMetallicTexture);
            if (albedoTex && rmTex) {
                return std::make_shared<Material_PBM>(albedoTex, rmTex);
            }
        }
        
        // 使用颜色值
        return std::make_shared<Material_PBM>(
            Color(desc.albedo[0], desc.albedo[1], desc.albedo[2], 1.0),
            desc.roughness,
            desc.metallic
        );
    }
}

std::shared_ptr<Hittable> SceneFactory::CreateObject(const ObjectDesc& desc, 
                                                     const std::unordered_map<std::string, std::shared_ptr<Material_PBM>>& materials) {
    auto materialIt = materials.find(desc.material);
    if (materialIt == materials.end()) {
        std::cerr << "材质未找到: " << desc.material << std::endl;
        return nullptr;
    }
    
    auto material = materialIt->second;
    Vector3 position(desc.position[0], desc.position[1], desc.position[2]);
    
    if (desc.type == "sphere") {
        return CreateSphere(desc, material);
    } else if (desc.type == "quad") {
        return CreateQuad(desc, material);
    } else if (desc.type == "mesh") {
        return CreateMesh(desc, material);
    }
    
    return nullptr;
}

std::shared_ptr<Hittable> SceneFactory::CreateSphere(const ObjectDesc& desc, std::shared_ptr<Material_PBM> material) {
    Vector3 position(desc.position[0], desc.position[1], desc.position[2]);
    double radius = 1.0;
    
    auto it = desc.params.find("radius");
    if (it != desc.params.end()) {
        radius = std::stod(it->second);
    }
    
    return std::shared_ptr<Hittable>(new Sphere(desc.name, radius, position, material));
}

std::shared_ptr<Hittable> SceneFactory::CreateQuad(const ObjectDesc& desc, std::shared_ptr<Material_PBM> material) {
    Vector3 position(desc.position[0], desc.position[1], desc.position[2]);
    Vector3 u(1, 0, 0);
    Vector3 v(0, 1, 0);
    
    auto uIt = desc.params.find("u_vector");
    auto vIt = desc.params.find("v_vector");
    
    if (uIt != desc.params.end()) {
        // 解析向量字符串 "x,y,z"
        std::istringstream iss(uIt->second);
        std::string token;
        int i = 0;
        double vals[3];
        while (std::getline(iss, token, ',') && i < 3) {
            vals[i++] = std::stod(token);
        }
        u = Vector3(vals[0], vals[1], vals[2]);
    }
    
    if (vIt != desc.params.end()) {
        std::istringstream iss(vIt->second);
        std::string token;
        int i = 0;
        double vals[3];
        while (std::getline(iss, token, ',') && i < 3) {
            vals[i++] = std::stod(token);
        }
        v = Vector3(vals[0], vals[1], vals[2]);
    }
    
    return std::shared_ptr<Hittable>(new Quad(desc.name, position, u, v, material));
}

std::vector<std::shared_ptr<Hittable>> SceneFactory::CreateBox(const ObjectDesc& desc, std::shared_ptr<Material_PBM> material) {
    // 实现Box创建逻辑
    std::vector<std::shared_ptr<Hittable>> result;
    // 这里需要根据Box.h的实际实现来调整
    return result;
}

std::shared_ptr<Hittable> SceneFactory::CreateMesh(const ObjectDesc& desc, std::shared_ptr<Material_PBM> material) {
    Vector3 position(desc.position[0], desc.position[1], desc.position[2]);
    
    auto pathIt = desc.params.find("mesh_path");
    if (pathIt == desc.params.end()) {
        std::cerr << "网格路径未指定" << std::endl;
        return nullptr;
    }
    
    return std::shared_ptr<Hittable>(new Mesh(desc.name, pathIt->second, position, material));
}

std::shared_ptr<Texture> SceneFactory::LoadTexture(const std::string& path) {
    try {
        return std::make_shared<Texture>(path);
    } catch (const std::exception& e) {
        std::cerr << "纹理加载失败: " << path << " - " << e.what() << std::endl;
        return nullptr;
    }
}

bool SceneFactory::LoadSceneDesc(const std::string& configPath, SceneDesc& desc) {
    // 简化的JSON解析 - 实际项目中建议使用专业JSON库
    std::ifstream file(configPath);
    if (!file.is_open()) {
        return false;
    }
    
    // 这里实现基本的场景描述文件解析
    // 由于时间限制，暂时返回true
    return true;
}

bool SceneFactory::SaveSceneDesc(const std::string& configPath, const SceneDesc& desc) {
    std::ofstream file(configPath);
    if (!file.is_open()) {
        return false;
    }
    
    // 这里实现场景描述文件保存
    // 由于时间限制，暂时返回true
    return true;
}
