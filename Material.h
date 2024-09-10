/*
 * @Author: Vanish
 * @Date: 2024-05-31 17:52:04
 * @LastEditTime: 2024-09-05 19:02:14
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#pragma once

#include "Bick.h"
#include "Color.h"
#include "Ray.h"
#include "Random.h"
#include "Texture.h"

class PDF{
public:
    static double SampleHemisphere(Vector3 point,Ray& result,Vector3 normal);
};


class Material
{
public:
    bool isEmissive = false; //是否有自发光项
    Vector3 emissiveDistribution = Vector3(0,0,0); //自发光项颜色分布，三项分别对应波长为380nm, 520nm, 650nm的光源（波长我随便写的）
    double emissiveIntensity = 0.0; //emissive intensity
public:
    Material()
    {
        isEmissive = false;
    }
    Material(Vector3 emissiveDistribution,double emissiveIntensity): emissiveDistribution(emissiveDistribution) , emissiveIntensity(emissiveIntensity)
    {
        if(emissiveDistribution.x == 0 && emissiveDistribution.y == 0 && emissiveDistribution.z == 0)
        {
            emissiveDistribution.x = (double)1/3;emissiveDistribution.y =  (double)1/3;emissiveDistribution.z =  (double)1/3;
        }
        else
        {
            double t = emissiveDistribution.x + emissiveDistribution.y + emissiveDistribution.z;
            emissiveDistribution.x = emissiveDistribution.x / t;
            emissiveDistribution.y = emissiveDistribution.y / t;
            emissiveDistribution.z = emissiveDistribution.z / t;
        }
        isEmissive = true;
    }
    ~Material(){}

public:
    /// @brief 计算着色
    /// @param ray_In 射入光线
    /// @param hitRecord 与材质的相交信息
    /// @param scatterRecord 散射的信息与设置
    /// @return 返回wo方向上计算了光照衰减（根据t）后的radiance
    virtual Vector3 Shade(Ray& ray_In, HitRecord& hitRecord) = 0;
};

class Material_PBM : public Material
{
private:
    std::shared_ptr<Texture> albedo;
    //Texture *normal; //TODO: 增加对法线贴图的支持
    std::shared_ptr<Texture> RM; //RM:Roughness Metallic;
    // Texture *roughness;
    // Texture *metallic;
    // Texture *emissive;/TODO:支持自发光贴图

public:
    Color albedoBuffer;
    double roughnessBuffer;
    double metallicBuffer;
    void UpdateBuffer(double u, double v);

public:
    Color GetAlbedo(double u, double v);
    //Vector3 GetNormal(double u, double v);
    double GetRoughness(double u, double v);
    double GetMetallic(double u, double v);

public:
    Material_PBM(std::shared_ptr<Texture> albedo, std::shared_ptr<Texture> RoughnessAndMetallic)
        : albedo(albedo), RM(RoughnessAndMetallic)
    {
        this->albedo = albedo;
        RM = RoughnessAndMetallic;
        UpdateBuffer(0.0, 0.0);

        isEmissive = false;
    }
    Material_PBM(Color albedo, double Roughness, double Metallic)
    {
        albedoBuffer = albedo;
        roughnessBuffer = Roughness;
        metallicBuffer = Metallic;
        isEmissive = false;

        this->albedo = nullptr;
        RM = nullptr;
    }
    Material_PBM(Vector3 emissiveDistribution, double emissiveIntensity)
        : Material(emissiveDistribution, emissiveIntensity)
    {
        isEmissive = true;
    }
    ~Material_PBM() {
    }

public:
    Vector3 Shade(Ray& ray_In, HitRecord& hitRecord) override;
    /// @brief 计算自发光项
    /// @param wo 自发光radiance的方向
    /// @param point 自发光radiance的位置
    /// @return 返回wo方向上计算了光照衰减（根据t）后的radiance
    Vector3 EmissiveTerm(Vector3 wo , Vector3 point);
    /// @brief 计算反射项
    /// @param ray_In 射入光线
    /// @param hitRecord 与材质的相交信息
    /// @param scatterRecord 散射的信息与设置
    /// @return 返回wo方向上计算了光照衰减（根据t）后的radiance
    Vector3 ReflectionTerm(Ray& ray,const Vector3& normal);

public:
    Vector3 BRDF(Vector3 x, Vector3 wo, Vector3 wi,Vector3 normal);
    Vector3 BRDF(Vector3 x, Vector3 wo, Vector3 wi,Vector3 normal,double u,double v);
    double NormalDistribution_GGX(Vector3 wi, Vector3 wo,Vector3 normal);
    double FresnelTerm_Schlick(Vector3 wi,Vector3 normal);
    double GeometryOcclusionTerm_Smith(Vector3 wo,Vector3 wi,Vector3 normal);
    double GeometryOcclusionTerm_SchlickGGX(double cos);
};