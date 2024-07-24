/*
 * @Author: Vanish
 * @Date: 2024-06-01 20:54:31
 * @LastEditTime: 2024-07-14 04:40:41
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#pragma once

#include "Hittable.h"

/// @brief 隐式表达的四边形类；定义position为起始点；u，v为两个非单位方向向量，表示方向与长度；normal存储了法向量
class Quad :public Hittable
{
public:
    Vector3 startPoint;
    Vector3 u,v;
    Vector3 normal; 
    double D;

public:
    Quad(){}
    Quad(std::string name,const Vector3 startPoint,const Vector3 u,const Vector3 v,std::shared_ptr<Material> material):
        Hittable(name,startPoint+(u+v)/2,material),u(u),v(v)
        {
            this->startPoint = startPoint;
            Vector3 n =u.Cross(v);
            normal =n.Normalized();
            D = Vector3::Dot(startPoint,normal); // 计算平面方程的常数项
        }
    ~Quad(){};
public:
    void Hit(Ray& ray ,const Interval interval,HitRecord& hitRecord) override
    {
        // Ray-Quad intersection algorithm
        // 第零步，不与背对的Plane相交
        float dotProduct =ray.direction.Dot(normal);
        if (dotProduct > 0)// 1e-6是个极小数，表示平行的阈值
        {
            hitRecord.hitted = false;
            return;
        }

        // 第一步，求射线与平面的交点
        double t = (D - Vector3::Dot(ray.origin,normal)) / dotProduct;
        if(!interval.Surrounds(t)){
            hitRecord.hitted = false;
            return;
        }

        // 第二步，判断交点是否在四边形内
        Vector3 intersectionPoint = ray.at(t);
        isInterior(intersectionPoint,hitRecord);
        if(!hitRecord.hitted)
        {
            return;
        }

        // 设置其他信息
        hitRecord.t = t;
        hitRecord.normal = normal;//Vector3::Dot(ray.direction,normal) < 0? Vector3(0,0,0)-normal : normal;
        hitRecord.hitPoint = intersectionPoint;
        hitRecord.isFrontFace =Vector3::Dot(ray.direction,normal) < 0? false : true;
        hitRecord.material = material;
        hitRecord.obj = this;
        ray.t = t;

        //if(hitRecord->material.get()->isEmissive) std::cout<<"光源被采样"<<std::endl;

        return;
    }
private:
    void isInterior(const Vector3& point , HitRecord& hitRecord) const
    {
        // 判断点是否在四边形内
        Vector3 planar_HitPoint = point - startPoint;
        double alpha = Vector3::Dot(planar_HitPoint,u)/Vector3::Dot(u,u);
        double beta = Vector3::Dot(planar_HitPoint,v)/Vector3::Dot(v,v);

        Interval interval = Interval(0,1);
        if(!interval.Surrounds(alpha) || !interval.Surrounds(beta)){
            hitRecord.hitted = false;
            return;
        }
        hitRecord.hitted = true;
        hitRecord.u = alpha;
        hitRecord.v = beta;
        return;
    }
};

inline std::vector<Quad*> Box(const Vector3& a, const Vector3& b, std::shared_ptr<Material> material,std::string name)
{
        auto min = Vector3(std::fmin(a.x,b.x), std::fmin(a.y,b.y), std::fmin(a.z,b.z));
        auto max = Vector3(std::fmax(a.x,b.x), std::fmax(a.y,b.y), std::fmax(a.z,b.z));

        auto dx = Vector3(max.x - min.x, 0, 0);
        auto dy = Vector3(0, max.y - min.y, 0);
        auto dz = Vector3(0, 0, max.z - min.z);

        Quad* quad1 = new Quad( name + "_front"  ,Vector3(min.x,min.y,min.z),dy,dx,material);
        Quad* quad2 = new Quad( name + "_right"  ,Vector3(max.x,min.y,min.z),dy,dz,material);
        Quad* quad3 = new Quad( name + "_back"   ,Vector3(min.x,min.y,max.z),dx,dy,material);
        Quad* quad4 = new Quad( name + "_left"   ,Vector3(min.x,min.y,min.z),dz,dy,material);
        Quad* quad5 = new Quad( name + "_top"    ,Vector3(min.x,max.y,min.z),dz,dx,material);
        Quad* quad6 = new Quad( name + "_bottom" ,Vector3(min.x,min.y,min.z),dx,dz,material);

        std::vector<Quad*> quads = {quad1,quad2,quad3,quad4,quad5,quad6};
        return quads;

}