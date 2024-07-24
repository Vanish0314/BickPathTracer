/*
 * @Author: Vanish
 * @Date: 2024-05-31 03:57:21
 * @LastEditTime: 2024-07-14 01:50:33
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#pragma once


#include <memory>

#include "Vector3.h"
#include "Material.h"
#include "Scene.h"
#include "Bick.h"
#include "Debuger.h"

class HitRecord
{
public: 
    bool      hitted; //是否击中物体
    Vector3   hitPoint; //击中点
    Vector3   normal; //法线
    double    t;//击中距离
    double    u;//纹理坐标u
    double    v;//纹理坐标v
    bool      isFrontFace;//是否是正面
    std::shared_ptr<Material> material = nullptr;//材质
    Hittable* obj; //击中的物体
    

public:
    HitRecord(bool hitted):hitted(hitted),hitPoint(Vector3(0,0,0)),normal(Vector3(0,0,0)),t(-1),u(-1),v(-1),isFrontFace(false),material(nullptr){}
    HitRecord(bool _hitted, Vector3 _hitPoint, Vector3 _normal, double _t, double _u, double _v, std::shared_ptr<Material> _material,Hittable* obj,bool _isFrontFace):hitted(_hitted),hitPoint(_hitPoint),normal(_normal),t(_t),u(_u),v(_v),isFrontFace(_isFrontFace),material(_material),obj(obj){}
    HitRecord():hitted(false),hitPoint(Vector3(0,0,0)),normal(Vector3(0,0,0)),t(-1),u(-1),v(-1),isFrontFace(false),material(nullptr){}
    ~HitRecord(){}

public:
    void Configure(bool _hitted, Vector3 _hitPoint, Vector3 _normal, double _t, double _u, double _v, std::shared_ptr<Material> _material,Hittable* obj,bool _isFrontFace){
        hitted      = _hitted;
        hitPoint    = _hitPoint;
        normal      = _normal;
        t           = _t;
        u           = _u;
        v           = _v;
        isFrontFace = _isFrontFace;
        material    = _material;
        this->obj   = obj;
    }
    void Configure(const HitRecord& other){
        hitted      = other.hitted;
        hitPoint    = other.hitPoint;
        normal      = other.normal;
        t           = other.t;
        u           = other.u;
        v           = other.v;
        isFrontFace = other.isFrontFace;
        material    = other.material;
        obj         = other.obj;
    }
};

class Interval{
public:
    double tmin = 0, tmax = __FLT_MAX__;
public:
    Interval():tmin(0),tmax(__FLT_MAX__){}
    Interval(double _tmin, double _tmax):tmin(_tmin),tmax(_tmax){}
    Interval(double tmin):tmin(tmin),tmax(__FLT_MAX__){}
public:
    bool Surrounds(double t) const { return (tmin <= t && t <= tmax); }
    bool Surrounds(Interval other) const { return (tmin <= other.tmin && other.tmax <= tmax); }
    bool Surrounds(int i) const { return (tmin <= i && i <= tmax); }
};

class Ray
{
public:
    int     depth       = 0;
    Vector3 origin;
    Vector3 direction;
    double  t           = -1;//在方向上行进的距离
public:
    Ray(Vector3 _origin, Vector3 _direction):origin(_origin),direction(_direction){}
    Ray(const Ray& other):depth(other.depth),origin(other.origin),direction(other.direction),t(other.t){}
    ~Ray(){}
public:
    Vector3 at(double t) const { return origin + t * direction; }
public:
    /// @brief 返回光线结果
    /// @param interval 
    /// @return 
    Vector3 Trace(Interval interval);
};
