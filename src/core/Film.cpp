/*
 * @Author: Vanish
 * @Date: 2024-09-03 18:48:08
 * @LastEditTime: 2024-09-08 05:42:19
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#include "Film.h"
#include "Profiler.h"

void Film::Save_P3(const std::string& filename)
{
    Profiler profiler("文件写入时间：");

    std::ofstream file(filename, std::ios::binary);
    file << "P3\n" << width << ' ' << height << "\n255\n";

    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            auto pixel = GetPixel(x, y);
            
            //ToneMapping_Reinhard
            pixel.x = pixel.x / (pixel.x + 1.0);
            pixel.y = pixel.y / (pixel.y + 1.0);
            pixel.z = pixel.z / (pixel.z + 1.0);

            //GammaCorrection
            pixel.x = std::pow(pixel.x, 0.45);
            pixel.y = std::pow(pixel.y, 0.45);
            pixel.z = std::pow(pixel.z, 0.45);
            
            //Clamp
            pixel.x = std::min(pixel.x,1.0);
            pixel.y = std::min(pixel.y,1.0);
            pixel.z = std::min(pixel.z,1.0);
            
            //Convert to int
            int r = (int)(pixel.x * 255);
            int g = (int)(pixel.y * 255);
            int b = (int)(pixel.z * 255);

            file << r << ' ' << g <<' ' << b <<'\n';
        }
    }   
}

void Film::Save_P3_WithoutCorrection(const std::string& filename)
{
    Profiler profiler("文件写入时间：");

    std::ofstream file(filename, std::ios::binary);
    file << "P3\n" << width << ' ' << height << "\n255\n";   

    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            auto pixel = GetPixel(x, y);
            file << pixel.x <<' ' << pixel.y <<' ' << pixel.z << '\n';
        }
    }   
}


void Film::Save_P6(const std::string& filename)
{
    this->GammaCorrection();
    this->ToneMapping_Reinhard();

    ThreadPool thread_pool(std::thread::hardware_concurrency());

    std::ofstream file(filename, std::ios::binary);
    file << "P6\n" << width << ' ' << height << "\n255\n";

    std::vector<uint8_t> buffer(width * height * 3);

    thread_pool.ParallelFor(width, height, [&](size_t x, size_t y) {
        auto pixel = GetPixel(x, y);
        Vector3 rgb(pixel);
        auto idx = (y * width + x) * 3;
        buffer[idx + 0] = rgb.x;
        buffer[idx + 1] = rgb.y;
        buffer[idx + 2] = rgb.z;
    });
    thread_pool.WaitAll();

    file.write(reinterpret_cast<const char *>(buffer.data()), buffer.size());
}

void Film::GammaCorrection()
{
    ThreadPool thread_pool(std::thread::hardware_concurrency());

    thread_pool.ParallelFor(width, height, [&](size_t x, size_t y) {
        auto pixel = GetPixel(x, y);
        pixel.x = std::pow(pixel.x, 1.0 / 2.2);
        pixel.y = std::pow(pixel.y, 1.0 / 2.2);
        pixel.z = std::pow(pixel.z, 1.0 / 2.2);
        SetPixel(x, y, pixel);
    });
    thread_pool.WaitAll();
}

void Film::ToneMapping_Reinhard()
{
    ThreadPool thread_pool(std::thread::hardware_concurrency());

    float L = 1.0;
    float c = 0.18;
    float max_white = 1.0;

    thread_pool.ParallelFor(width, height, [&](size_t x, size_t y) {
        auto pixel = GetPixel(x, y);
        float L_pixel = pixel.x + pixel.y + pixel.z;
        if (L_pixel > 0.0) {
            L_pixel = L_pixel / (3.0 * max_white);
            L_pixel = L_pixel * (1.0 + L_pixel * (c * L_pixel));
            L_pixel = L_pixel / (1.0 + L_pixel);
            L_pixel = L_pixel * max_white;
        }
        else {
            L_pixel = 0.0;
        }
        pixel.x = pixel.x * L / L_pixel;
        pixel.y = pixel.y * L / L_pixel;
        pixel.z = pixel.z * L / L_pixel;
        SetPixel(x, y, pixel);
    });
    thread_pool.WaitAll();
}


void DebugFilm::AddFilm(std::string name)
{
    Guard guard(spinLock);
    
    Film* film = new Film(width, height);
    filmMap->insert(std::pair<std::string, Film*>(name, film));
}

bool DebugFilm::HasFilm(std::string name)
{
    Guard guard(spinLock);

    return filmMap->find(name) != filmMap->end();
}

void DebugFilm::SetFilm(std::string name,int x,int y,const Vector3& color)
{
    Guard guard(spinLock);

    Film* film = filmMap->at(name);
    film->SetPixel(x,y,color);
}

void DebugFilm::Save_P3()
{
    for(auto it = filmMap->begin();it!=filmMap->end();it++)
    {
        it->second->Save_P3(it->first + ".ppm");
    }
}
