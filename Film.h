/*
 * @Author: Vanish
 * @Date: 2024-09-03 18:48:01
 * @LastEditTime: 2024-09-08 05:21:24
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#pragma once

#include "Vector3.h"
#include "SpinLock.h"
#include "ThreadPool.h"
#include <fstream>
#include <map>

struct Pixel {
    Vector3 color;
    int ssp;
};

/// @brief 胶片类
/// @details 保存的时RGB数据，每个像素点的颜色值(0-255)
class Film {
public:
    Film(int w, int h) : width(w), height(h) 
    {
        pixels = new std::vector<Pixel>(w * h);
    }
    virtual ~Film() {
        delete pixels;
    }

protected:
    int width, height;
    std::vector<Pixel> *pixels;
    SpinLock spinLock;
    

public:
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    Vector3 GetPixel(int x, int y) const
    {   
        return (*pixels)[y * width + x].color/(*pixels)[y * width + x].ssp; 
    }
    void SetPixel(int x, int y, const Vector3& color)
    {
        (*pixels)[y * width + x].color = color;
    }
    void AddSample(int x, int y, const Vector3& color)
    {
        (*pixels)[y * width + x].color += color;
        (*pixels)[y * width + x].ssp++;
    }

public:
    void Save_P3(const std::string& filename);
    void Save_P3_WithoutCorrection(const std::string& filename);
    void Save_P6(const std::string& filename);

private:
    void GammaCorrection();
    void ToneMapping_Reinhard();
};


class DebugFilm:public Film 
{
public:
    DebugFilm(int w, int h):Film(w,h){}
    ~DebugFilm(){}
    
public:
    std::map<std::string,Film*> *filmMap = new std::map<std::string,Film*>();

public:
    void AddFilm(std::string name);
    bool HasFilm(std::string name);
    void SetFilm(std::string name,int x,int y,const Vector3& color);
    void Save_P3();
};