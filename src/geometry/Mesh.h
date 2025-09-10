/*
 * @Author: Vanish
 * @Date: 2024-07-14 08:57:22
 * @LastEditTime: 2024-09-09 03:44:52
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#pragma once

class BVH;

#include <string>
#include <fstream>
#include <sstream>
#include "Triangle.h"
#include "BVH.h"
#include "Hittable.h"
#include "Vector2.h"
#include "rapidobj.hpp"
#include "Profiler.h"

class Mesh : public Hittable
{
public:

//     Mesh(std::string name, std::string path, Vector3 position, std::shared_ptr<Material> material)
//     : Hittable(name, position, material)
// {
//     std::vector<Triangle> *triangles = new std::vector<Triangle>;

//     std::ifstream objFile(path, std::ios::in);

//     if (!objFile.is_open())
//     {
//         std::cerr << "Error: cannot open file " << path << std::endl;
//         return;
//     }

//     // 去除注释
//     std::string line;
//     std::getline(objFile, line);
//     while (line[0] == '#')
//     {
//         std::getline(objFile, line);
//     }

//     // 去除文件名
//     std::getline(objFile, line);

//     auto *points  = new std::vector<Vector3>;
//     auto *normals = new std::vector<Vector3>;
//     auto *uvs     = new std::vector<Vector2>;

//     // 读取数据
//     while (std::getline(objFile, line) && line[0] != 'f')
//     {
//         std::istringstream iss(line);
//         std::string prefix;
//         iss >> prefix;

//         if (prefix == "v")
//         {
//             float x, y, z;
//             iss >> x >> y >> z;
//             points->push_back(Vector3(x, y, z));
//         }
//         else if (prefix == "vn")
//         {
//             float nx, ny, nz;
//             iss >> nx >> ny >> nz;
//             normals->push_back(Vector3(nx, ny, nz));
//         }
//         else if (prefix == "vt")
//         {
//             float u, v;
//             iss >> u >> v;
//             uvs->push_back(Vector2(u, v));
//         }
//     }

//     //TODO: 为Mesh添加旋转和放缩功能
//     //因为没有旋转和放缩，所以所有顶点变换到世界空间只需要加上位置即可,且手动放大两倍
//     for (int i = 0; i < points->size(); i++)
//     {
//         // 原模型面朝下，需要手动绕x轴旋转90度
//         points->at(i) = Vector3(points->at(i).x, points->at(i).z, -points->at(i).y);
//         points->at(i) = points->at(i) * 0.7;
//         points->at(i) += position;      
//     }

//     std::vector<Vector3> *f = new std::vector<Vector3>;
//     do
//     {
//         std::istringstream iss(line);
//         std::string prefix;
//         std::string v;
//         iss >> prefix;

//         for(int i = 0; i < 3; i++)
//         {
//             iss >> v;
//             std::replace(v.begin(), v.end(), '/', ' '); // 替换 '/' 为 ' '，以便于分割
//             std::istringstream iss2(v);

//             int x,y,z;
//             iss2>>x>> std::skipws>>z;
//             //iss>>y;
//             y = 0; //FIXME: 因为BlenderMonkey没有uv，这里先hardcode为0
//             f->push_back(Vector3(x,y,z));
//         }


//     } while (std::getline(objFile, line) && line[0] == 'f');

//     Vertex *vertices = new Vertex[f->size()];
//     triangles = new std::vector<Triangle>(f->size()/3);
//     for (int i = 0; i < f->size() /3; i ++)
//     {   
//         vertices[3 * i] = Vertex(
//             points->at(f->at(3 * i).x - 1),
//             normals->at(f->at(3 * i).z - 1),//uvs->at((f[3 * i].z - 1))
//             0//FIXME: 因为BlenderMonkey没有uv，这里先hardcode为0
//         );
//         vertices[3 * i + 1] = Vertex(
//             points->at(f->at(3 * i + 1).x- 1),
//             normals->at(f->at(3 * i + 1).z- 1),
//             //uvs->at(f[3 * i + 1].z- 1)
//             0//FIXME: 因为BlenderMonkey没有uv，这里先hardcode为0
//         );
//         vertices[3 * i + 2] = Vertex(
//             points->at(f->at(3 * i + 2).x- 1),
//             normals->at(f->at(3 * i + 2).z- 1),
//             //uvs->at(f[3 * i + 2].z- 1)
//             0//FIXME: 因为BlenderMonkey没有uv，这里先hardcode为0
//         );

//         triangles->push_back(Triangle(vertices[3 * i], vertices[3 * i + 1], vertices[3 * i + 2]));
//     }

//     delete points;
//     delete normals;
//     delete uvs;
//     delete f;
//     delete[] vertices;

//     bvh.Build(*triangles);
// }
    
    Mesh(std::string name,const std::filesystem::path &fileName,Vector3 position, std::shared_ptr<Material> material)
    : Hittable(name, position, material)
    {
        Profiler profiler("加载模型");

        auto result = rapidobj::ParseFile(fileName,rapidobj::MaterialLibrary::Ignore());
        std::vector<Triangle> *triangles = new std::vector<Triangle>;

        for (const auto &shape : result.shapes) {
        size_t index_offset = 0;
        for (size_t num_face_vectex : shape.mesh.num_face_vertices) {
            if (num_face_vectex == 3) {
                auto index = shape.mesh.indices[index_offset];
                Vector3 pos0 {
                    result.attributes.positions[index.position_index * 3 + 0],
                    result.attributes.positions[index.position_index * 3 + 1],
                    result.attributes.positions[index.position_index * 3 + 2]
                };
                index = shape.mesh.indices[index_offset + 1];
                Vector3 pos1 {
                    result.attributes.positions[index.position_index * 3 + 0],
                    result.attributes.positions[index.position_index * 3 + 1],
                    result.attributes.positions[index.position_index * 3 + 2]
                };
                index = shape.mesh.indices[index_offset + 2];
                Vector3 pos2 {
                    result.attributes.positions[index.position_index * 3 + 0],
                    result.attributes.positions[index.position_index * 3 + 1],
                    result.attributes.positions[index.position_index * 3 + 2]
                };

                if (index.normal_index >= 0) {
                    index = shape.mesh.indices[index_offset];
                    Vector3 normal0 {
                        result.attributes.normals[index.normal_index * 3 + 0],
                        result.attributes.normals[index.normal_index * 3 + 1],
                        result.attributes.normals[index.normal_index * 3 + 2]
                    };
                    index = shape.mesh.indices[index_offset + 1];
                    Vector3 normal1 {
                        result.attributes.normals[index.normal_index * 3 + 0],
                        result.attributes.normals[index.normal_index * 3 + 1],
                        result.attributes.normals[index.normal_index * 3 + 2]
                    };
                    index = shape.mesh.indices[index_offset + 2];
                    Vector3 normal2 {
                        result.attributes.normals[index.normal_index * 3 + 0],
                        result.attributes.normals[index.normal_index * 3 + 1],
                        result.attributes.normals[index.normal_index * 3 + 2]
                    };
                
                    double x ,z = 0;
                    x = -pos0.z;
                    z = pos0.x;
                    pos0 = Vector3(x, pos0.y, z);
                    x = -pos1.z;
                    z = pos1.x;
                    pos1 = Vector3(x, pos1.y, z);
                    x = -pos2.z;
                    z = pos2.x;
                    pos2 = Vector3(x, pos2.y, z);
                    triangles->push_back(Triangle 
                    (
                        Vertex(pos0 + position, normal0, Vector2(0, 0)),
                        Vertex(pos1 + position, normal1, Vector2(0, 0)),
                        Vertex(pos2 + position, normal2, Vector2(0, 0))
                    ));
                } else {

                    double x ,z = 0;
                    x = -pos0.z;
                    z = pos0.x;
                    pos0 = Vector3(x, pos0.y, z);
                    x = -pos1.z;
                    z = pos1.x;
                    pos1 = Vector3(x, pos1.y, z);
                    x = -pos2.z;
                    z = pos2.x;
                    pos2 = Vector3(x, pos2.y, z);
                    triangles->push_back(Triangle 
                    (
                        Vertex(pos0 + position, Vector3(0, 0, 0), Vector2(0, 0)),
                        Vertex(pos1 + position, Vector3(0, 0, 0), Vector2(0, 0)),
                        Vertex(pos2 + position, Vector3(0, 0, 0), Vector2(0, 0))
                    ));
                }
            }
            index_offset += num_face_vectex;
        }
    }
    bvh.Build(*triangles);

    }
    
    ~Mesh(){}

private:
    BVH bvh;

public:

    SampleResult UnitSamplePdf() override
    {
        return SampleResult();
    }

    void Hit (Ray& ray ,const Interval interval,HitRecord& hitRecord)
    override
    {
        HitRecord tempRecord;
        bvh.Hit(ray, interval, tempRecord);
        if(!tempRecord.hitted) 
            return;

        tempRecord.material = material;
        tempRecord.obj = this;
        hitRecord = tempRecord;
    }

};

