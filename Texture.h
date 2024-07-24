#pragma once

#include <string>
#include <fstream>
#include "Vector3.h"

class vec3
{
public:
    uint8_t r, g, b;
public:
    vec3(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
    vec3() : r(0), g(0), b(0) {}
public:
    Vector3 operator / (double scalar) const
    {
        return Vector3(r / scalar, g / scalar, b / scalar);
    }
};

class Texture
{
public:
    int width=512, height=512;
    std::vector<vec3> *data;//HardCoded for now
    std::string path;

public:
    Texture(std::string path){
        Load(path);
    }
    ~Texture(){
    }

private:

    void Load(std::string path)
    {
        this->path = path;
        data = new std::vector<vec3>();

        // 预分配固定大小的内存
        data->resize(width * height);

        // 打开文件
        std::ifstream file(path, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Could not open the file!" << std::endl;
            return;
        }

        std::string line;
        std::getline(file, line); // P6 Header
        if (line != "P6") {
            std::cerr << "Not a valid P6 PPM file!" << std::endl;
            return;
        }

        // 读取图像尺寸
        do {
            std::getline(file, line);
        } while (line[0] == '#'); // 跳过注释行

        // std::istringstream dimensions(line);
        // dimensions >> width >> height;

        // if (width != FixedWidth || height != FixedHeight) {
        //     std::cerr << "Image size is not 512x512!" << std::endl;
        //     return;
        // }

        // 读取最大颜色值
        std::getline(file, line);

        // 读取图像数据
        size_t dataSize = width * height * 3;
        auto buffer = new char[dataSize];
        file.read(buffer, dataSize);

        // 解析图像数据
        for (size_t i = 0, j = 0; i < dataSize; i += 3, ++j) {
            data->at(j) = vec3(buffer[i],buffer[i+1], buffer[i+2]);
        }

        file.close();
        delete[] buffer;
    }

public:
    Vector3 Sample_Point(double u, double v) const
    {   
        int x = (int)(u * 512);
        int y = (int)(v * 512);
        return data->at(x + y*512)/255.0;
    }
    Vector3 Sample_Bilinear(double u, double v) const
    {
        return Sample_Point(u, v);

        //TODO: Implement Bilinear Interpolation
        //int x = (int)(u * 512);
        // int y = (int)(v * 512);
        // double u_frac = u * 512 - x;
        // double v_frac = v * 512 - y;
        // double u_inv_frac = 1 - u_frac;
        // double v_inv_frac = 1 - v_frac;
        // Vector3 c00 = data[x + y*512];
        // Vector3 c10 = data[(x+1) + y*512];
        // Vector3 c01 = data[x + (y+1)*512];
        // Vector3 c11 = data[(x+1) + (y+1)*512];
        // return (c00 * u_inv_frac + c10 * u_frac) * v_inv_frac + (c01 * u_inv_frac + c11 * u_frac) * v_frac;
    }
};