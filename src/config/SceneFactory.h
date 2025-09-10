/*
 * @Author: Vanish
 * @Date: 2025-09-10 20:50:00
 * @LastEditTime: 2025-09-10 20:50:00
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

// 前向声明
class Scene;
class Material_PBM;
class Hittable;
class Texture;

enum class SceneType {
    CORNELL_BOX,
    PBR_DEMO,
    TEXTURE_DEMO,
    MESH_DEMO,
    CUSTOM
};

struct MaterialDesc {
    std::string name;
    std::string type = "pbr";  // pbr, emissive
    
    // PBR材质参数
    double albedo[3] = {1.0, 1.0, 1.0};
    double roughness = 0.5;
    double metallic = 0.0;
    std::string albedoTexture;
    std::string roughnessMetallicTexture;
    
    // 自发光参数
    double emission[3] = {0.0, 0.0, 0.0};
    double emissionIntensity = 0.0;
};

struct ObjectDesc {
    std::string name;
    std::string type;  // sphere, quad, box, mesh
    std::string material;
    
    // 变换参数
    double position[3] = {0.0, 0.0, 0.0};
    double rotation[3] = {0.0, 0.0, 0.0};
    double scale[3] = {1.0, 1.0, 1.0};
    
    // 几何体特定参数
    std::unordered_map<std::string, std::string> params;
};

struct SceneDesc {
    std::vector<MaterialDesc> materials;
    std::vector<ObjectDesc> objects;
    std::vector<ObjectDesc> lights;
};

class SceneFactory {
public:
    static std::unique_ptr<Scene> CreateScene(SceneType type);
    static std::unique_ptr<Scene> CreateSceneFromFile(const std::string& configPath);
    static std::unique_ptr<Scene> CreateSceneFromDesc(const SceneDesc& desc);
    
    // 预定义场景创建函数
    static std::unique_ptr<Scene> CreateCornellBoxScene();
    static std::unique_ptr<Scene> CreatePBRDemoScene();
    static std::unique_ptr<Scene> CreateTextureDemoScene();
    static std::unique_ptr<Scene> CreateMeshDemoScene();
    
    // 场景描述文件加载
    static bool LoadSceneDesc(const std::string& configPath, SceneDesc& desc);
    static bool SaveSceneDesc(const std::string& configPath, const SceneDesc& desc);

private:
    // 辅助函数
    static std::shared_ptr<Material_PBM> CreateMaterial(const MaterialDesc& desc);
    static std::shared_ptr<Hittable> CreateObject(const ObjectDesc& desc, 
                                                   const std::unordered_map<std::string, std::shared_ptr<Material_PBM>>& materials);
    static std::shared_ptr<Texture> LoadTexture(const std::string& path);
    
    // 几何体创建函数
    static std::shared_ptr<Hittable> CreateSphere(const ObjectDesc& desc, std::shared_ptr<Material_PBM> material);
    static std::shared_ptr<Hittable> CreateQuad(const ObjectDesc& desc, std::shared_ptr<Material_PBM> material);
    static std::vector<std::shared_ptr<Hittable>> CreateBox(const ObjectDesc& desc, std::shared_ptr<Material_PBM> material);
    static std::shared_ptr<Hittable> CreateMesh(const ObjectDesc& desc, std::shared_ptr<Material_PBM> material);
    static std::vector<class Quad*> CreateRotatedBox(const class Vector3& position, const class Vector3& size, 
                                                     const class Vector3& rotation, std::shared_ptr<Material_PBM> material, 
                                                     const std::string& name);
    
    // 场景类型字符串转换
    static SceneType StringToSceneType(const std::string& typeStr);
    static std::string SceneTypeToString(SceneType type);
};
