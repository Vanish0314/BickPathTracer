/*
 * @Author: Vanish
 * @Date: 2024-09-05 20:04:04
 * @LastEditTime: 2024-09-05 21:06:47
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#pragma once

class Triangle;

#include "Vector3.h"
#include "Bounds.h"
#include "Triangle.h"

struct BVHNode {

    Bounds bounds {};
    std::vector<Triangle> triangles;
    BVHNode* left = nullptr;
    BVHNode* right = nullptr;

    void UpdateBounds()
    {
        bounds = {};
        for (int i = 0; i < triangles.size(); i++) {
            bounds.Expand(triangles[i].v1.position);
            bounds.Expand(triangles[i].v2.position);
            bounds.Expand(triangles[i].v3.position);
        }
    }
};

class BVH : public Hittable
{
public:
    void Build(std::vector<Triangle> &triangles);

private:
    void RecursiveSplit(BVHNode* node); 
    void RecursiveIntersect(BVHNode *node,const Ray &ray,Interval interval,HitRecord &hitRecord);

private:
    BVHNode *root = nullptr;

public:
    virtual void Hit(Ray& ray ,const Interval interval,HitRecord& hitRecord) override
    {
        RecursiveIntersect(root,ray,interval,hitRecord);
    }
    virtual SampleResult UnitSamplePdf() override{return SampleResult{};};
};