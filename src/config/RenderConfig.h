/*
 * @Author: Vanish
 * @Date: 2025-09-10 20:40:00
 * @LastEditTime: 2025-09-10 20:40:00
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#pragma once

#include <string>
#include <iostream>

struct RenderSettings {
    int sampleCount = 1024;
    int imageWidth = 512;
    int imageHeight = 512;
    double russianRouletteProb = 0.7;
    int threadCount = 0; // 0 = auto detect
    bool enableImportanceSampling = false;
    bool enableSAH = false;
    std::string outputPath = "output.ppm";
    
    // Debug settings
    bool debugMode = false;
    std::string debugOutputDir = "output/debug/";
    bool renderDebugImages = false;
    bool showRenderProgress = true;
};

struct CameraSettings {
    double position[3] = {2.8, 2.5, -2.6};
    double direction[3] = {0, 0, 1};
    double up[3] = {0, 1, 0};
    double fov = 60.0;
};

class RenderConfig {
public:
    RenderSettings render;
    CameraSettings camera;
    std::string sceneConfigPath;
    
public:
    RenderConfig() = default;
    
    // 从JSON文件加载配置
    bool LoadFromFile(const std::string& configPath);
    
    // 从命令行参数解析配置
    bool ParseCommandLine(int argc, char* argv[]);
    
    // 保存配置到文件
    bool SaveToFile(const std::string& configPath) const;
    
    // 打印当前配置
    void PrintConfig() const;
    
    // 验证配置的有效性
    bool Validate() const;

private:
    void PrintUsage() const;
    bool ParseResolution(const std::string& resolution);
};
