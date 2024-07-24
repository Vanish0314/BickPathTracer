/*
 * @Author: Vanish
 * @Date: 2024-06-02 04:28:38
 * @LastEditTime: 2024-07-14 06:24:18
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#include "Material.h"


double PDF::SampleHemisphere(Vector3 point,Ray& result,Vector3 normal)
{

    Vector3 randomVec = Vector3(0,0,0);
    while (randomVec.Dot(normal) <= 0)
    {
        randomVec = Vector3(
            Random::GetRandomDouble(-1,1),
            Random::GetRandomDouble(-1,1),
            Random::GetRandomDouble(-1,1)
        );
    }
    
    result.origin = point + normal * 0.01;
    result.direction = randomVec.Normalized();
    result.depth++;
    result.t = 0;

    double PDF_Term = 1 / (2 * PI);
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
    return result ;// (1 / (2 * PI));
}

Vector3 Material_PBM::EmissiveTerm(Vector3 wo , Vector3 point)
{
    #ifdef DEBUG_TRACERAY
    if(Random::consoleOutPutEnable && isEmissive)
        std::cout <<"\033[34m"<<"【【【!采样光源!】】】"<<"\033[0m";
    #endif
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
    /*
        Cook-Torrance BRDF 方程
        f = kd*f_lanbert + ks*f_cooktorrance

        f_lanbert = albedoBuffer / pi
        f_cooktorrance= D * F * G / denom

        D: 法线分布函数
        F: 菲涅尔方程
        G: 几何遮蔽函数
        denom: 辐射分母 = 4 * Dot(wo, normal) * Dot(wi, normal)
    */

    Vector3 Lambert;
    Lambert.x = albedoBuffer.r / PI;
    Lambert.y = albedoBuffer.g / PI;
    Lambert.z = albedoBuffer.b / PI;
    
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
    double alpha = roughnessBuffer * roughnessBuffer;
    double alpha2 = alpha * alpha;
    double cos = Vector3::Dot(wi, normal);
    double cos2 = cos * cos;

    double num = alpha2;
    double denom = (cos2 * (alpha2 - 1) + 1);
    denom = PI * denom * denom;

    return num / denom;
}

double Material_PBM::FresnelTerm_Schlick(Vector3 wi,Vector3 normal)
{
    // Schlick近似公式 f = f0 + (1-f0)(1-cosθ)^5
    // 其中：
    // f0:  金属度 ，位于0.04-1之间
    // θ: 入射光线与法线夹角

    double f0 = 0.04;
    f0 = f0 + (1 - f0) * metallicBuffer;

    return f0 + (1 - f0) * std::pow(1 - Vector3::Dot(wi, normal), 5);
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