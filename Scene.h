/*
 * @Author: Vanish
 * @Date: 2024-05-31 03:57:21
 * @LastEditTime: 2024-09-04 14:53:44
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#pragma once

#include "Bick.h"
#include "Hittable.h"
#include "Ray.h"

class Scene
{
public:
    std::vector<Hittable*> objects;// 所有物体
    std::vector<Hittable*> hittables;// 不发光的物体
    std::vector<Hittable*> lights;// 发光物体
public:
    Scene();
    ~Scene();
public:
    void AddObject(Hittable* object);
    int GetObjectCount() const {return objects.size();};
    Hittable* GetObject(int index) const {return objects[index];};
public:
    void Hit(Ray& ray, Interval interval, HitRecord &hitRecord) const;
};
