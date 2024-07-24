/*
 * @Author: Vanish
 * @Date: 2024-05-31 03:57:21
 * @LastEditTime: 2024-07-14 06:35:20
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#pragma once

#include "Hittable.h"
#include "Vector3.h"

class Sphere:public Hittable
{
public:
    float radius;
public:
    Sphere(std::string name,float r,Vector3 position, std::shared_ptr<Material> material):Hittable(name,position,material),radius(r) {}
    ~Sphere(){};
public:

void Hit (Ray& ray ,const Interval interval,HitRecord& hitRecord)
    override {

        // Solve t^2 * d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
        // where 
        //  o: sphere center, 
        //  p: ray origin, 
        //  d: ray direction, 
        //  R: sphere radius,t: 光线行进距离
        //
        //  a = d.d
        //  b = 2*(o-p).d 
        //  c = (o-p).(o-p)-R^2
        
        Vector3 oc = ray.origin - position; // o-p
	    float a = Vector3::Dot(ray.direction, ray.direction);
	    float b = 2.0f * Vector3::Dot(ray.direction, oc);
	    float c = Vector3::Dot(oc, oc) - radius*radius;
        
        // Calculate the discriminant
        // discriminant = b^2 - 4ac
        float discriminant = b * b - 4 * a * c;
        if (discriminant < 0) {
            hitRecord.hitted = false;
            return;
        }

        // Calculate the two possible intersection points
        // t = (-b +- sqrt(discriminant)) / (2*a)
        float t1 = (-b - std::sqrt(discriminant)) / (2 * a);
        float t2 = (-b + std::sqrt(discriminant)) / (2 * a);

        if(t1<0 || t2<0)
        {
            hitRecord.hitted = false;
            return;
        }

        // 判断t是否在interval范围内
        float t = std::min(t1, t2);
        if(!interval.Surrounds(t))
        {
            if(interval.Surrounds(std::max(t1,t2)))
            {
                t = std::max(t1,t2);
            }
            else
            {
                hitRecord.hitted = false;
                return;
            }
        }

        hitRecord.t = t;
        ray.t = t;

        //Configure the hit record
        hitRecord.hitted = true;
        hitRecord.hitPoint = ray.at(hitRecord.t);
        hitRecord.normal = (hitRecord.hitPoint - position).Normalized();    
        hitRecord.material = material;
        hitRecord.isFrontFace = (ray.direction.Dot(hitRecord.normal) < 0);
        hitRecord.u = GetUAt(hitRecord.hitPoint);
        hitRecord.v = GetVAt(hitRecord.hitPoint);
        hitRecord.obj = this;
    }
private:
    double GetUAt(const Vector3& hitPoint) const;
    double GetVAt(const Vector3& hitPoint) const;
};