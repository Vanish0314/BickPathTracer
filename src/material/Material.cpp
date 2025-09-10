/*
 * @Author: Vanish
 * @Date: 2024-06-02 04:28:38
 * @LastEditTime: 2024-09-08 05:05:03
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#include "Material.h"
#include "../BickCommon.h"


double PDF::SampleHemisphere(Vector3 point,Ray& result,Vector3 normal)
{
    // 余弦加权半球采样
    double r1 = Random::GetRandomDouble(0, 1);
    double r2 = Random::GetRandomDouble(0, 1);
    
    // 极坐标采样
    double cosTheta = sqrt(r1);
    double sinTheta = sqrt(1 - r1);
    double phi = 2 * BickConstants::PI * r2;
    
    // 局部坐标系
    Vector3 w = normal;
    Vector3 u = ((abs(w.x) > 0.1) ? Vector3(0, 1, 0) : Vector3(1, 0, 0)).Cross(w).Normalized();
    Vector3 v = w.Cross(u);
    
    // 转换到世界坐标
    Vector3 direction = u * (cos(phi) * sinTheta) + v * (sin(phi) * sinTheta) + w * cosTheta;
    
    result.origin = point + normal * 1e-4;
    result.direction = direction.Normalized();
    result.depth++;
    result.t = 0;

    // 余弦加权采样的PDF = cosθ/π (physically correct)
    double PDF_Term = cosTheta / BickConstants::PI;
    return PDF_Term;
}


Vector3 Material_PBM::Shade(Ray& ray_In, HitRecord & hitRecord)
{
    Vector3 x = ray_In.at(ray_In.t);
    Vector3 wo = Vector3(0,0,0) - ray_In.direction;
    Vector3 normal = hitRecord.normal;
    double r = ray_In.t;
    UpdateBuffer(hitRecord.u, hitRecord.v);

    Vector3 result = Vector3(0, 0, 0);

    result = EmissiveTerm(x, wo) + ReflectionTerm(ray_In, normal);

    if(result.Magnitude() == 0) return Vector3(0,0,0);

    double attenuation = 1.0/(std::pow(r, 1.7));//计算衰减
    result = result* (attenuation<1?attenuation:1);//防止能量爆炸
    return result ;// (1 / (2 * BickConstants::PI));
}

Vector3 Material_PBM::EmissiveTerm(Vector3 /* wo */ , Vector3 /* point */)
{
    // Debug output removed - use proper logging system if needed
    return emissiveDistribution * emissiveIntensity;
}

Vector3 Material_PBM::ReflectionTerm(Ray& ray,const Vector3& normal)
{   
    Vector3 wo = Vector3(0,0,0) - ray.direction;
    Vector3 x = ray.at(ray.t);

    Vector3 Li_Term = Vector3(0, 0, 0);
    Vector3 result = Vector3(0, 0, 0);

    double PDF_Term = PDF::SampleHemisphere(x,ray,normal);
    Vector3 wi = ray.direction;

    Vector3 BRDF_Term = BRDF(x, wo, wi,normal);
    double  Cos_Term = std::max(0.0, Vector3::Dot(wi, normal));
    Li_Term = ray.Trace(Interval());
    result = BRDF_Term * Li_Term * Cos_Term;  
    //result += 0.03 * Vector3(albedoBuffer.r, albedoBuffer.g, albedoBuffer.b) * ao;//环境光遮蔽

    if(result.Magnitude() == 0) return Vector3(0,0,0);
    return result;
}
Vector3 Material_PBM::BRDF(Vector3 x, Vector3 wo, Vector3 wi,Vector3 normal)
{
    // 对于Cornell box场景，使用简单的Lambertian BRDF
    // 如果roughness接近1.0且metallic接近0.0，则使用纯漫反射
    if (roughnessBuffer >= 0.9 && metallicBuffer <= 0.1) {
        // Pure Lambertian diffuse BRDF
        Vector3 Lambert;
        Lambert.x = albedoBuffer.r / BickConstants::PI;
        Lambert.y = albedoBuffer.g / BickConstants::PI;
        Lambert.z = albedoBuffer.b / BickConstants::PI;
        return Lambert;
    }
    
    /*
        Cook-Torrance BRDF 方程
        f = kd*f_lanbert + ks*f_cooktorrance

        f_lanbert = albedoBuffer / pi (physically correct)
        f_cooktorrance= D * F * G / denom

        D: 法线分布函数
        F: 菲涅尔方程
        G: 几何遮蔽函数
        denom: 辐射分母 = 4 * Dot(wo, normal) * Dot(wi, normal)
    */

    Vector3 Lambert;
    Lambert.x = albedoBuffer.r / BickConstants::PI;
    Lambert.y = albedoBuffer.g / BickConstants::PI;
    Lambert.z = albedoBuffer.b / BickConstants::PI;
    
    double D = NormalDistribution_GGX(wi, wo, normal);
    double F = FresnelTerm_Schlick(wi, normal);
    double G = GeometryOcclusionTerm_Smith(wo, wi, normal);
    double denom = 4 * Vector3::Dot(wo, normal) * Vector3::Dot(wi, normal) + 0.000001;//避免除0
    
    double ks = F;
    double kd = 1 - ks;
    kd *= (1 - metallicBuffer);

    double CookTorrance_Term = D * F * G / denom;
    Vector3 result = kd*Lambert + Vector3(CookTorrance_Term, CookTorrance_Term, CookTorrance_Term);
    result = Vector3(
        std::min(result.x, 1.0),
        std::min(result.y, 1.0),
        std::min(result.z, 1.0)
    );//防止能量爆炸

    return result;
}
Vector3 Material_PBM::BRDF(Vector3 x, Vector3 wo, Vector3 wi,Vector3 normal,double u, double v)
{
    UpdateBuffer(u, v);
    /*
        Cook-Torrance BRDF 方程
        f = kd*f_lanbert + ks*f_cooktorrance

        f_lanbert = albedoBuffer / pi (physically correct)
        f_cooktorrance= D * F * G / denom

        D: 法线分布函数
        F: 菲涅尔方程
        G: 几何遮蔽函数
        denom: 辐射分母 = 4 * Dot(wo, normal) * Dot(wi, normal)
    */

    Vector3 Lambert;
    Lambert.x = albedoBuffer.r / BickConstants::PI;
    Lambert.y = albedoBuffer.g / BickConstants::PI;
    Lambert.z = albedoBuffer.b / BickConstants::PI;
    
    double D = NormalDistribution_GGX(wi, wo, normal);
    double F = FresnelTerm_Schlick(wi, normal);
    double G = GeometryOcclusionTerm_Smith(wo, wi, normal);
    double denom = 4 * Vector3::Dot(wo, normal) * Vector3::Dot(wi, normal) + 0.000001;//避免除0
    
    double ks = F;
    double kd = 1 - ks;
    kd *= (1 - metallicBuffer);

    double CookTorrance_Term = D * F * G / denom;
    Vector3 result = kd*Lambert + Vector3(CookTorrance_Term, CookTorrance_Term, CookTorrance_Term);
    result = Vector3(
        std::min(result.x, 1.0),
        std::min(result.y, 1.0),
        std::min(result.z, 1.0)
    );//防止能量爆炸

    return result;
}
double Material_PBM::NormalDistribution_GGX(Vector3 wi, Vector3 wo,Vector3 normal)
{
    // 计算半向量
    Vector3 h = (wi + wo).Normalized();
    double NdotH = std::max(0.0, Vector3::Dot(normal, h));
    
    double alpha = roughnessBuffer * roughnessBuffer;
    double alpha2 = alpha * alpha;
    double NdotH2 = NdotH * NdotH;

    double num = alpha2;
    double denom = (NdotH2 * (alpha2 - 1) + 1);
    denom = BickConstants::PI * denom * denom;

    return num / (denom + 1e-7); // 避免除零
}

double Material_PBM::FresnelTerm_Schlick(Vector3 wi,Vector3 normal)
{
    // Schlick近似公式 f = f0 + (1-f0)(1-cosθ)^5
    // 使用半向量计算菲涅尔项
    Vector3 wo = Vector3(0, 0, 0) - wi; // 观察方向
    Vector3 h = (wi + wo).Normalized();
    double HdotV = std::max(0.0, Vector3::Dot(h, wo));

    double f0 = 0.04;
    f0 = f0 + (1 - f0) * metallicBuffer;

    return f0 + (1 - f0) * std::pow(1 - HdotV, 5);
}

double Material_PBM::GeometryOcclusionTerm_Smith(Vector3 wo,Vector3 wi,Vector3 normal)
{
    //Schlick近似公式: g = g1(θi) * g1(θo)
    
    double cosi = Vector3::Dot(wi, normal);
    double coso = Vector3::Dot(wo, normal);
    double gi = GeometryOcclusionTerm_SchlickGGX(cosi);
    double go = GeometryOcclusionTerm_SchlickGGX(coso);

    return gi * go;
}

double Material_PBM::GeometryOcclusionTerm_SchlickGGX(double cos)
{
    double r = roughnessBuffer +1;
    double k = r*r/8;
    
    double num = cos;
    double denom = cos * (1-k) + k;

    return num/denom;
}

Color Material_PBM::GetAlbedo(double u, double v)
{
    Vector3 a = albedo->Sample_Bilinear(u, v);

    return Color(
        a.x,
        a.y,
        a.z,
        1
    );
        
}

double Material_PBM::GetMetallic(double u, double v)
{
    Vector3 m = RM->Sample_Bilinear(u, v);

    return m.y;
}

double Material_PBM::GetRoughness(double u, double v)
{
    Vector3 r = RM->Sample_Bilinear(u, v);

    return r.x;
}

void Material_PBM::UpdateBuffer(double u, double v)
{
    if(albedo == nullptr || RM == nullptr) return;
    albedoBuffer = GetAlbedo(u, v);
    metallicBuffer = GetMetallic(u, v);
    roughnessBuffer = GetRoughness(u, v);
}