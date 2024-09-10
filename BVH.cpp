/*
 * @Author: Vanish
 * @Date: 2024-09-05 20:09:15
 * @LastEditTime: 2024-09-05 21:17:04
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#include "BVH.h"
#include "Profiler.h"

void BVH::Build(std::vector<Triangle> &triangles)
{
    Profiler profiler("模型(面数：" + std::to_string(triangles.size()) + ")BVH构造时间");

    root = new BVHNode{};
    root->triangles = std::move(triangles);
    root->UpdateBounds();
    RecursiveSplit(root);
}

void BVH::RecursiveSplit(BVHNode *node)
{
    if(node->triangles.size() == 1)
    {
        return;
    }
    if(node->triangles.size() == 2)
    {

    }

    auto diag = node->bounds.GetDiagonal();
    auto axis = diag.x>diag.y? (diag.x>diag.z? 0 : 2) : (diag.y>diag.z? 1 : 2);
    float mid = node->bounds.min[axis] + diag[axis] * 0.5f;
    std::vector<Triangle> left, right;

    for(auto &tri : node->triangles)
    {
        if(tri.v1.position[axis] + tri.v2.position[axis] + tri.v3.position[axis] < mid)
        {
            left.push_back(tri);
        }
        else
        {

            right.push_back(tri);
        }
    }

    if(left.empty() || right.empty())
    {
        return;
    }

    auto leftNode = new BVHNode{};
    auto rightNode = new BVHNode{};
    node->left = leftNode;
    node->right = rightNode;
    node->triangles.clear();
    node->triangles.shrink_to_fit();
    leftNode->triangles = std::move(left);
    rightNode->triangles = std::move(right);
    leftNode->UpdateBounds();
    rightNode->UpdateBounds();

    RecursiveSplit(leftNode);
    RecursiveSplit(rightNode);
}

void BVH::RecursiveIntersect(BVHNode *node,const Ray &ray,Interval interval,HitRecord &hitRecord)
{
    if(!node->bounds.Intersect(ray,interval.tmin,interval.tmax))
        return;

    if(node->triangles.empty())
    {
        RecursiveIntersect(node->left,ray,interval,hitRecord);
        RecursiveIntersect(node->right,ray,interval,hitRecord);
    }
    else
    {
        for(auto &tri : node->triangles)
        {
            HitRecord tempHitRecord;
            tri.Intersect(ray,interval,tempHitRecord);
            if(tempHitRecord.hitted)
            {
                if(tempHitRecord.t < hitRecord.t || !hitRecord.hitted)
                {
                    hitRecord = tempHitRecord;
                }
            }
        }
    }
}