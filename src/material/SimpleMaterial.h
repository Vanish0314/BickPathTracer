/*
 * @Author: Vanish
 * @Date: 2025-09-10 23:30:00
 * @LastEditTime: 2025-09-10 23:30:00
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */

#pragma once
#include "../math/Vector3.h"
#include "../math/Color.h"
#include "../core/Ray.h"
#include "../BickCommon.h"
#include <memory>
#include <optional>

// 简化的材质系统，基于参考项目的正确实现
struct BSDFSample {
    Vector3 bsdf;
    double pdf;
    Vector3 light_direction;
    double eta_scale = 1.0;
};

// 坐标系变换帮助类
class LocalFrame {
public:
    Vector3 x_axis, y_axis, z_axis;
    
    LocalFrame(const Vector3& normal) {
        y_axis = normal.Normalized();
        Vector3 up = (abs(y_axis.y) < 0.99999) ? Vector3(0, 1, 0) : Vector3(0, 0, 1);
        x_axis = up.Cross(y_axis).Normalized();
        z_axis = x_axis.Cross(y_axis).Normalized();
    }
    
    Vector3 localFromWorld(const Vector3& direction_world) const {
        return Vector3(
            Vector3::Dot(direction_world, x_axis),
            Vector3::Dot(direction_world, y_axis), 
            Vector3::Dot(direction_world, z_axis)
        ).Normalized();
    }
    
    Vector3 worldFromLocal(const Vector3& direction_local) const {
        return (x_axis * direction_local.x + y_axis * direction_local.y + z_axis * direction_local.z).Normalized();
    }
};

// 球面采样函数
inline Vector3 CosineSampleHemisphere(double u1, double u2) {
    double r = sqrt(u1);
    double phi = 2 * BickConstants::PI * u2;
    return Vector3(r * cos(phi), sqrt(1 - u1), r * sin(phi));
}

inline double CosineSampleHemispherePDF(const Vector3& direction) {
    return direction.y / BickConstants::PI;
}

// 简化的漫反射材质
class SimpleDiffuseMaterial {
public:
    Vector3 albedo;
    bool isEmissive;
    Vector3 emission;
    
    SimpleDiffuseMaterial(const Vector3& albedo) : albedo(albedo), isEmissive(false) {}
    SimpleDiffuseMaterial(const Vector3& emission, bool emissive) : emission(emission), isEmissive(true) {}
    
    std::optional<BSDFSample> sampleBSDF(const Vector3& hit_point, const Vector3& view_direction) const {
        if (abs(view_direction.y) < 1e-6) {
            return std::nullopt;
        }
        
        double u1 = Random::GetRandomDouble(0, 1);
        double u2 = Random::GetRandomDouble(0, 1);
        Vector3 light_direction = CosineSampleHemisphere(u1, u2);
        
        // 确保方向在正确的半球
        if (view_direction.y < 0) {
            light_direction.y = -light_direction.y;
        }
        
        double pdf = CosineSampleHemispherePDF(light_direction);
        Vector3 bsdf = albedo * (1.0 / BickConstants::PI);
        
        return BSDFSample{ bsdf, pdf, light_direction };
    }
    
    Vector3 BSDF(const Vector3& hit_point, const Vector3& light_direction, const Vector3& view_direction) const {
        if (light_direction.y * view_direction.y <= 0) {
            return Vector3(0, 0, 0);
        }
        return albedo * (1.0 / BickConstants::PI);
    }
    
    double PDF(const Vector3& hit_point, const Vector3& light_direction, const Vector3& view_direction) const {
        if (light_direction.y * view_direction.y <= 0) {
            return 0;
        }
        return CosineSampleHemispherePDF(light_direction);
    }
    
    Vector3 getEmission() const {
        return isEmissive ? emission : Vector3(0, 0, 0);
    }
    
    bool isDeltaDistribution() const {
        return false;
    }
};
