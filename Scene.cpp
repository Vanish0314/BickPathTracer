/*
 * @Author: Vanish
 * @Date: 2024-05-31 04:37:49
 * @LastEditTime: 2024-09-05 21:31:12
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#include "Scene.h"
#include "Profiler.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::AddObject(Hittable* obj)
{
	objects.push_back(obj);
	if(obj->material.get()->isEmissive){
		lights.push_back(obj);
	}
	else hittables.push_back(obj);
}

void Scene::Hit(Ray &ray, Interval interval, HitRecord &hitRecord) const
{

	for (int i = 0; i < GetObjectCount(); i++)
	{
		Hittable* obj = objects[i];

		HitRecord hit;
		obj->Hit(ray, interval, hit);
		if (interval.Surrounds(hit.t))
		{
			if (hitRecord.t < 0 || hit.t < hitRecord.t)
				hitRecord.Configure(hit);
		}
	}
	if (hitRecord.t < 0)
		hitRecord.hitted = false;
}
