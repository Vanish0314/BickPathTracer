/*
 * @Author: Vanish
 * @Date: 2025-09-10 21:00:00
 * @LastEditTime: 2025-09-10 21:00:00
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */

#include "BickPathTracer.h"
#include <chrono>

int main(int argc, char* argv[]) {
    std::cout << "BickPathTracer - 软路径追踪渲染器" << std::endl;
    std::cout << "Author: Vanish" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 解析配置
    RenderConfig config;
    
    // 如果没有提供命令行参数，使用默认配置
    if (argc == 1) {
        std::cout << "使用默认配置渲染PBR演示场景..." << std::endl;
        config.sceneConfigPath = "pbr_demo";  // 内置场景
    } else {
        if (!config.ParseCommandLine(argc, argv)) {
            return 1;
        }
    }
    
    // 验证配置
    if (!config.Validate()) {
        std::cerr << "配置验证失败!" << std::endl;
        return 1;
    }
    
    // 打印配置信息
    config.PrintConfig();
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // 创建渲染器
    Renderer renderer(config);
    
    // 初始化渲染器
    std::cout << "初始化渲染器..." << std::endl;
    if (!renderer.Initialize()) {
        std::cerr << "渲染器初始化失败!" << std::endl;
        return 1;
    }
    
    // 开始渲染
    std::cout << "开始渲染..." << std::endl;
    if (!renderer.Render()) {
        std::cerr << "渲染失败!" << std::endl;
        return 1;
    }
    
    // 清理资源
    renderer.Cleanup();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // 格式化输出耗时
    int ms = duration.count();
    if (ms < 1000) {
        std::cout << "渲染完成! 耗时: " << ms << "ms" << std::endl;
    } else if (ms < 60000) {
        std::cout << "渲染完成! 耗时: " << (ms / 1000.0) << "s" << std::endl;
    } else if (ms < 3600000) {
        int minutes = ms / 60000;
        int seconds = (ms % 60000) / 1000;
        std::cout << "渲染完成! 耗时: " << minutes << "min " << seconds << "s" << std::endl;
    } else {
        int hours = ms / 3600000;
        int minutes = (ms % 3600000) / 60000;
        int seconds = (ms % 60000) / 1000;
        std::cout << "渲染完成! 耗时: " << hours << "h " << minutes << "min " << seconds << "s" << std::endl;
    }
    
    return 0;
}
