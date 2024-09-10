/*
 * @Author: Vanish
 * @Date: 2024-09-05 20:55:38
 * @LastEditTime: 2024-09-06 09:51:54
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#pragma once

#include "Vector3.h"
#include "Vector2.h"
#include "Ray.h"

struct Vertex
{
public:
    Vector3 position;
    Vector3 normal;
    Vector2 uv;
public:
    Vertex(){}
    Vertex(const Vector3& position, const Vector3& normal, const Vector2& uv) : position(position), normal(normal), uv(uv) {}
    ~Vertex(){}
};  


class Triangle
{
public:
    Vertex v1, v2, v3;
public:
    Triangle(){}
    Triangle(Vertex v1, Vertex v2, Vertex v3):v1(v1), v2(v2), v3(v3){}
    ~Triangle(){}

public:
   
    void Intersect(const Ray& ray, const Interval& interval, HitRecord& hitRecord) const
    {
        // 利用重心坐标求解

        // 边向量
        Vector3 e1 = v2.position - v1.position;
        Vector3 e2 = v3.position - v1.position;
        
        Vector3 p = ray.direction.Cross(e2);
        double determinant = Vector3::Dot(e1, p);

        Vector3 T;
        if(determinant >0)
        {
            T = ray.origin - v1.position;
        }
        else
        {
            T = v1.position - ray.origin;
            determinant = -determinant;
        }
        if(determinant < 0.00001) // 平行不考虑
        {
            hitRecord.hitted =false;
            return;
        }

        double u = Vector3::Dot(T, p);
        if(u < 0 || u > determinant)// 超出范围不考虑
        {
            hitRecord.hitted =false;
            return;
        }
        
        Vector3 Q = T.Cross(e1);
        double v = Vector3::Dot(ray.direction, Q);
        if(v < 0 || u + v > determinant) // 超出范围不考虑
        {
            hitRecord.hitted =false;
            return;
        }

        double t = Vector3::Dot(e2, Q);
        double inv_determinant = 1.0 / determinant;
        t *= inv_determinant;
        u *= inv_determinant;
        v *= inv_determinant;

        if(!interval.Surrounds(t))
        {
            hitRecord.hitted =false;
            return;
        }

        //配置数据,只配置必要的减小消耗
        hitRecord.hitted = true;
        hitRecord.t = t;
        hitRecord.hitPoint = ray.at(t);
        hitRecord.normal = v1.normal == Vector3(0,0,0) ? v2.normal == Vector3(0,0,0) ? v3.normal : v2.normal : v1.normal;
        hitRecord.isFrontFace = true;
        //FIXME: 此uv非彼uv,但因为BlenderMonkey没有uv，所以这里送它一个
        hitRecord.u = u;
        hitRecord.v = v;
    }
};