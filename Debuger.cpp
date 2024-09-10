/*
 * @Author: Vanish
 * @Date: 2024-07-14 01:17:14
 * @LastEditTime: 2024-09-03 20:21:47
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#include "Debuger.h"

bool Debuger::DebugMode()
{
#ifdef RENDER_DEBUG_UV
    return true;
#endif
#ifdef RENDER_DEBUG_NORMAL
    return true;
#endif
#ifdef RENDER_DEBUG_HITPOINT
    return true;
#endif
#ifdef RENDER_DEBUG_DEPTH
    return true;
#endif
#ifdef RENDER_DEBUG_RAYGEN
    return true;
#endif
#ifdef RENDER_DEBUG_TRACE_RAY
    return true;
#endif
#ifdef RENDER_DEBUG_SAMPLE
    return true;
#endif
#ifdef RENDER_DEBUG_SHOW_ALBEDO
    return true;
#endif
#ifdef RENDER_DEBUG_SHOW_ROUGHNESS
    return true;
#endif
#ifdef RENDER_DEBUG_SHOW_METALLIC
    return true;
#endif
#ifdef RENDER_DEBUG_REFLECTION
    return true;
#endif

return false;
}

Vector3 Debuger::DebugResult(Ray& ray)
{
#ifdef RENDER_DEBUG_UV
    HitRecord hitRecord;
    g_Scene->Hit(ray, Interval(),hitRecord);
    if(!hitRecord.hitted) return Vector3(0,0,0);
    Vector3 res = Vector3(
        hitRecord.u,
        hitRecord.v,
        0
    );
    return res * 255;
#endif

#ifdef RENDER_DEBUG_NORMAL
    HitRecord hitRecord;
    g_Scene->Hit(ray, Interval(),hitRecord);
    if(!hitRecord.hitted) return Vector3(0,0,0);
    Vector3 res = Vector3(
        std::abs(hitRecord.normal.x),
        std::abs(hitRecord.normal.y),
        std::abs(hitRecord.normal.z)
    );
    return res * 255;
#endif

#ifdef RENDER_DEBUG_HITPOINT
    HitRecord hitRecord;
    g_Scene->Hit(ray, Interval(),hitRecord);
    if(!hitRecord.hitted) return Vector3(0,0,0);
    Vector3 res = Vector3(
        hitRecord.hitPoint.x,
        hitRecord.hitPoint.y,
        hitRecord.hitPoint.z
    );
    return res * 30;
#endif

#ifdef RENDER_DEBUG_DEPTH
    HitRecord hitRecord;
    g_Scene->Hit(ray, Interval(),hitRecord);
    if(!hitRecord.hitted) return Vector3(0,0,0);
    
    int depth = ray.t;
    depth = std::pow(depth, 10);
    depth = (int)(depth /(250000 * 8));

    Vector3 res = Vector3(
        depth,
        depth,
        depth
    );
    return res;
#endif

#ifdef RENDER_DEBUG_RAYGEN
    return Vector3(
        std::abs(ray.direction.x),
        std::abs(ray.direction.y),
        std::abs(ray.direction.z)
    ) * 255;
#endif

#ifdef RENDER_DEBUG_TRACE_RAY
    
    int bounceCountForRed = 0;
    int 

    HitRecord hitRecord;
    g_Scene->Hit(ray, Interval(),hitRecord);
    if(!hitRecord.hitted) return Vector3(0,0,0);

#endif

#ifdef RENDER_DEBUG_SAMPLE
    HitRecord hitRecord;
    g_Scene->Hit(ray, Interval(),hitRecord);
    if(!hitRecord.hitted) return Vector3(0,0,0);
    Vector3 res = Vector3(
        hitRecord.normal.x,
        hitRecord.normal.y,
        hitRecord.normal.z
    );

    Vector3 randomVec = Vector3(0,0,0);
    while (randomVec.Dot(res) <= 0)
    {
        randomVec = Vector3(
            Random::GetRandomDouble(-1,1),
            Random::GetRandomDouble(-1,1),
            Random::GetRandomDouble(-1,1)
        );
    }
    
    randomVec = Vector3(
        std::max(randomVec.x, 0.0),
        std::max(randomVec.y, 0.0),
        std::max(randomVec.z, 0.0)
    );

    return randomVec * 255;

#endif

#ifdef RENDER_DEBUG_SHOW_ALBEDO
    HitRecord hitRecord;
    g_Scene->Hit(ray, Interval(),hitRecord);
    if(!hitRecord.hitted) return Vector3(0,0,0);
    auto m = hitRecord.material;
    //转换m为Material_PBM
    auto pbm = dynamic_cast<Material_PBM*>(m.get());
    pbm->UpdateBuffer(hitRecord.u,hitRecord.v);
    Color albedo = pbm->albedoBuffer;
    return Vector3(albedo.r, albedo.g, albedo.b) * 255;
#endif

#ifdef RENDER_DEBUG_SHOW_ROUGHNESS
    HitRecord hitRecord;
    g_Scene->Hit(ray, Interval(),hitRecord);
    if(!hitRecord.hitted) return Vector3(0,0,0);
    auto m = hitRecord.material;
    //转换m为Material_PBM
    auto pbm = dynamic_cast<Material_PBM*>(m.get());
    pbm->UpdateBuffer(hitRecord.u,hitRecord.v);
    double roughness = pbm->roughnessBuffer;
    return Vector3(roughness, roughness, roughness) * 255;
#endif

#ifdef RENDER_DEBUG_SHOW_METALLIC
    HitRecord hitRecord;
    g_Scene->Hit(ray, Interval(),hitRecord);
    if(!hitRecord.hitted) return Vector3(0,0,0);
    auto m = hitRecord.material;
    //转换m为Material_PBM
    auto pbm = dynamic_cast<Material_PBM*>(m.get());
    pbm->UpdateBuffer(hitRecord.u,hitRecord.v);
    double metalic = pbm->metallicBuffer;
    return Vector3(metalic, metalic, metalic) * 255;
#endif

#ifdef RENDER_DEBUG_REFLECTION
    
#endif

}