/*
 * @Author: Vanish
 * @Date: 2024-07-14 08:57:22
 * @LastEditTime: 2024-07-14 15:08:11
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include "Hittable.h"
#include "Vector2.h"

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
};
class Mesh : public Hittable
{
public:
    

public:
Mesh(std::string name, std::string path, Vector3 position, std::shared_ptr<Material> material)
    : Hittable(name, position, material)
{
    std::ifstream objFile(path, std::ios::in);

    if (!objFile.is_open())
    {
        std::cerr << "Error: cannot open file " << path << std::endl;
        return;
    }

    // 去除注释
    std::string line;
    std::getline(objFile, line);
    while (line[0] == '#')
    {
        std::getline(objFile, line);
    }

    // 去除文件名
    std::getline(objFile, line);

    auto *points  = new std::vector<Vector3>;
    auto *normals = new std::vector<Vector3>;
    auto *uvs     = new std::vector<Vector2>;

    // 读取数据
    while (std::getline(objFile, line) && line[0] != 'f')
    {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v")
        {
            float x, y, z;
            iss >> x >> y >> z;
            points->push_back(Vector3(x, y, z));
        }
        else if (prefix == "vn")
        {
            float nx, ny, nz;
            iss >> nx >> ny >> nz;
            normals->push_back(Vector3(nx, ny, nz));
        }
        else if (prefix == "vt")
        {
            float u, v;
            iss >> u >> v;
            uvs->push_back(Vector2(u, v));
        }
    }

    //TODO: 为Mesh添加旋转和放缩功能
    //因为没有旋转和放缩，所以所有顶点变换到世界空间只需要加上位置即可,且手动放大两倍
    for (int i = 0; i < points->size(); i++)
    {
        // 原模型面朝下，需要手动绕x轴旋转90度
        points->at(i) = Vector3(points->at(i).x, points->at(i).z, -points->at(i).y);
        points->at(i) = points->at(i) * 0.7;
        points->at(i) += position;      
    }

    std::vector<Vector3> *f = new std::vector<Vector3>;
    do
    {
        std::istringstream iss(line);
        std::string prefix;
        std::string v;
        iss >> prefix;

        for(int i = 0; i < 3; i++)
        {
            iss >> v;
            std::replace(v.begin(), v.end(), '/', ' '); // 替换 '/' 为 ' '，以便于分割
            std::istringstream iss2(v);

            int x,y,z;
            iss2>>x>> std::skipws>>z;
            //iss>>y;
            y = 0; //FIXME: 因为BlenderMonkey没有uv，这里先hardcode为0
            f->push_back(Vector3(x,y,z));
        }


    } while (std::getline(objFile, line) && line[0] == 'f');

    Vertex *vertices = new Vertex[f->size()];
    triangles = new std::vector<Triangle>(f->size()/3);
    for (int i = 0; i < f->size() /3; i ++)
    {   
        vertices[3 * i] = Vertex(
            points->at(f->at(3 * i).x - 1),
            normals->at(f->at(3 * i).z - 1),//uvs->at((f[3 * i].z - 1))
            0//FIXME: 因为BlenderMonkey没有uv，这里先hardcode为0
        );
        vertices[3 * i + 1] = Vertex(
            points->at(f->at(3 * i + 1).x- 1),
            normals->at(f->at(3 * i + 1).z- 1),
            //uvs->at(f[3 * i + 1].z- 1)
            0//FIXME: 因为BlenderMonkey没有uv，这里先hardcode为0
        );
        vertices[3 * i + 2] = Vertex(
            points->at(f->at(3 * i + 2).x- 1),
            normals->at(f->at(3 * i + 2).z- 1),
            //uvs->at(f[3 * i + 2].z- 1)
            0//FIXME: 因为BlenderMonkey没有uv，这里先hardcode为0
        );

        triangles->push_back(Triangle(vertices[3 * i], vertices[3 * i + 1], vertices[3 * i + 2]));
    }

    delete points;
    delete normals;
    delete uvs;
    delete f;
    delete[] vertices;
}

    ~Mesh(){}

private:
    std::vector<Triangle> *triangles;

public:
    void Hit (Ray& ray ,const Interval interval,HitRecord& hitRecord)
    override
    {
        Triangle triangle;
        HitRecord result;
        result.t = 0;
        HitRecord tempRecord;
        Interval tempInterval = interval;
        for(int i = 0; i < triangles->size(); i++)
        {
            triangle = triangles->at(i);
            TriangleIntersect(triangle, ray, tempInterval, tempRecord);
            if(tempRecord.hitted)
            {
                result = tempRecord;
                result.normal = triangle.v1.normal;//TODO:完善这个三角形的算法，这里直接取一个顶点的
                tempInterval.tmin = tempRecord.t;
            }
        }

        if(result.t > 0)
        {
            hitRecord.hitted = true;
            hitRecord.t = result.t;
            hitRecord.u = result.u;
            hitRecord.v = result.v;
            hitRecord.hitPoint = ray.at(hitRecord.t);
            hitRecord.normal = result.normal;
            hitRecord.material = material;
            hitRecord.obj = this;
            hitRecord.isFrontFace = true;
        }
        else
        {
            hitRecord.hitted = false;
            hitRecord.t = -1;
        }
    }


private:
    void TriangleIntersect(Triangle triangle, Ray& ray, const Interval& interval, HitRecord& hitRecord)
    {
        // 利用重心坐标求解

        // 边向量
        Vector3 e1 = triangle.v2.position - triangle.v1.position;
        Vector3 e2 = triangle.v3.position - triangle.v1.position;
        
        Vector3 p = ray.direction.Cross(e2);
        double determinant = Vector3::Dot(e1, p);

        Vector3 T;
        if(determinant >0)
        {
            T = ray.origin - triangle.v1.position;
        }
        else
        {
            T = triangle.v1.position - ray.origin;
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
        //FIXME: 此uv非彼uv,但因为BlenderMonkey没有uv，所以这里送它一个
        hitRecord.u = u;
        hitRecord.v = v;
    }
};

