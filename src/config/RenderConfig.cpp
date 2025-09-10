/*
 * @Author: Vanish
 * @Date: 2025-09-10 20:40:00
 * @LastEditTime: 2025-09-10 20:40:00
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */

#include "RenderConfig.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>

bool RenderConfig::LoadFromFile(const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open config file: " << configPath << std::endl;
        return false;
    }

    // 简单的JSON解析 (这里为了简化，使用基本的字符串解析)
    // 在实际项目中建议使用专业的JSON库如nlohmann/json
    std::string line;
    while (std::getline(file, line)) {
        // 移除空格和注释
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        if (line.empty() || line[0] == '/' || line[0] == '#') continue;
        
        // 解析键值对
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;
        
        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);
        
        // 移除引号和逗号
        key.erase(std::remove(key.begin(), key.end(), '"'), key.end());
        value.erase(std::remove(value.begin(), value.end(), '"'), value.end());
        value.erase(std::remove(value.begin(), value.end(), ','), value.end());
        
        // 解析配置项
        if (key == "sample_count") {
            render.sampleCount = std::stoi(value);
        } else if (key == "image_width") {
            render.imageWidth = std::stoi(value);
        } else if (key == "image_height") {
            render.imageHeight = std::stoi(value);
        } else if (key == "russian_roulette_prob") {
            render.russianRouletteProb = std::stod(value);
        } else if (key == "thread_count") {
            render.threadCount = std::stoi(value);
        } else if (key == "enable_importance_sampling") {
            render.enableImportanceSampling = (value == "true");
        } else if (key == "enable_sah") {
            render.enableSAH = (value == "true");
        } else if (key == "output_path") {
            render.outputPath = value;
        } else if (key == "debug_mode") {
            render.debugMode = (value == "true");
        } else if (key == "debug_output_dir") {
            render.debugOutputDir = value;
        } else if (key == "render_debug_images") {
            render.renderDebugImages = (value == "true");
        } else if (key == "show_render_progress") {
            render.showRenderProgress = (value == "true");
        } else if (key == "camera_position") {
            // 解析数组 [x,y,z]
            value.erase(std::remove(value.begin(), value.end(), '['), value.end());
            value.erase(std::remove(value.begin(), value.end(), ']'), value.end());
            std::istringstream iss(value);
            std::string token;
            int i = 0;
            while (std::getline(iss, token, ',') && i < 3) {
                camera.position[i++] = std::stod(token);
            }
        } else if (key == "camera_direction") {
            value.erase(std::remove(value.begin(), value.end(), '['), value.end());
            value.erase(std::remove(value.begin(), value.end(), ']'), value.end());
            std::istringstream iss(value);
            std::string token;
            int i = 0;
            while (std::getline(iss, token, ',') && i < 3) {
                camera.direction[i++] = std::stod(token);
            }
        } else if (key == "camera_up") {
            value.erase(std::remove(value.begin(), value.end(), '['), value.end());
            value.erase(std::remove(value.begin(), value.end(), ']'), value.end());
            std::istringstream iss(value);
            std::string token;
            int i = 0;
            while (std::getline(iss, token, ',') && i < 3) {
                camera.up[i++] = std::stod(token);
            }
        } else if (key == "camera_fov") {
            camera.fov = std::stod(value);
        } else if (key == "scene_config") {
            sceneConfigPath = value;
        }
    }
    
    return true;
}

bool RenderConfig::ParseCommandLine(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--config" && i + 1 < argc) {
            if (!LoadFromFile(argv[++i])) {
                return false;
            }
        } else if (arg == "--samples" && i + 1 < argc) {
            render.sampleCount = std::stoi(argv[++i]);
        } else if (arg == "--resolution" && i + 1 < argc) {
            if (!ParseResolution(argv[++i])) {
                return false;
            }
        } else if (arg == "--output" && i + 1 < argc) {
            render.outputPath = argv[++i];
        } else if (arg == "--threads" && i + 1 < argc) {
            render.threadCount = std::stoi(argv[++i]);
        } else if (arg == "--debug") {
            render.debugMode = true;
        } else if (arg == "--debug-images") {
            render.renderDebugImages = true;
        } else if (arg == "--debug-output" && i + 1 < argc) {
            render.debugOutputDir = argv[++i];
        } else if (arg == "--scene" && i + 1 < argc) {
            sceneConfigPath = argv[++i];
        } else if (arg == "--help" || arg == "-h") {
            PrintUsage();
            return false;
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            PrintUsage();
            return false;
        }
    }
    
    return true;
}

bool RenderConfig::SaveToFile(const std::string& configPath) const {
    std::ofstream file(configPath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot create config file: " << configPath << std::endl;
        return false;
    }
    
    file << "{\n";
    file << "  \"render_settings\": {\n";
    file << "    \"sample_count\": " << render.sampleCount << ",\n";
    file << "    \"image_width\": " << render.imageWidth << ",\n";
    file << "    \"image_height\": " << render.imageHeight << ",\n";
    file << "    \"russian_roulette_prob\": " << render.russianRouletteProb << ",\n";
    file << "    \"thread_count\": " << render.threadCount << ",\n";
    file << "    \"enable_importance_sampling\": " << (render.enableImportanceSampling ? "true" : "false") << ",\n";
    file << "    \"enable_sah\": " << (render.enableSAH ? "true" : "false") << ",\n";
    file << "    \"output_path\": \"" << render.outputPath << "\",\n";
    file << "    \"debug_mode\": " << (render.debugMode ? "true" : "false") << ",\n";
    file << "    \"debug_output_dir\": \"" << render.debugOutputDir << "\",\n";
    file << "    \"render_debug_images\": " << (render.renderDebugImages ? "true" : "false") << ",\n";
    file << "    \"show_render_progress\": " << (render.showRenderProgress ? "true" : "false") << "\n";
    file << "  },\n";
    file << "  \"camera\": {\n";
    file << "    \"camera_position\": [" << camera.position[0] << "," << camera.position[1] << "," << camera.position[2] << "],\n";
    file << "    \"camera_direction\": [" << camera.direction[0] << "," << camera.direction[1] << "," << camera.direction[2] << "],\n";
    file << "    \"camera_up\": [" << camera.up[0] << "," << camera.up[1] << "," << camera.up[2] << "],\n";
    file << "    \"camera_fov\": " << camera.fov << "\n";
    file << "  },\n";
    file << "  \"scene_config\": \"" << sceneConfigPath << "\"\n";
    file << "}\n";
    
    return true;
}

void RenderConfig::PrintConfig() const {
    std::cout << "=== Render Configuration ===" << std::endl;
    std::cout << "Sample Count: " << render.sampleCount << std::endl;
    std::cout << "Resolution: " << render.imageWidth << "x" << render.imageHeight << std::endl;
    std::cout << "Russian Roulette Probability: " << render.russianRouletteProb << std::endl;
    std::cout << "Thread Count: " << (render.threadCount == 0 ? "Auto" : std::to_string(render.threadCount)) << std::endl;
    std::cout << "Importance Sampling: " << (render.enableImportanceSampling ? "Enabled" : "Disabled") << std::endl;
    std::cout << "SAH: " << (render.enableSAH ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Output Path: " << render.outputPath << std::endl;
    std::cout << "Debug Mode: " << (render.debugMode ? "Enabled" : "Disabled") << std::endl;
    if (render.debugMode) {
        std::cout << "Debug Output Dir: " << render.debugOutputDir << std::endl;
        std::cout << "Render Debug Images: " << (render.renderDebugImages ? "Yes" : "No") << std::endl;
    }
    std::cout << "Camera Position: [" << camera.position[0] << ", " << camera.position[1] << ", " << camera.position[2] << "]" << std::endl;
    std::cout << "Camera Direction: [" << camera.direction[0] << ", " << camera.direction[1] << ", " << camera.direction[2] << "]" << std::endl;
    std::cout << "Camera Up: [" << camera.up[0] << ", " << camera.up[1] << ", " << camera.up[2] << "]" << std::endl;
    std::cout << "Camera FOV: " << camera.fov << std::endl;
    if (!sceneConfigPath.empty()) {
        std::cout << "Scene Config: " << sceneConfigPath << std::endl;
    }
    std::cout << "==============================" << std::endl;
}

bool RenderConfig::Validate() const {
    if (render.sampleCount <= 0) {
        std::cerr << "Error: Sample count must be positive" << std::endl;
        return false;
    }
    
    if (render.imageWidth <= 0 || render.imageHeight <= 0) {
        std::cerr << "Error: Image resolution must be positive" << std::endl;
        return false;
    }
    
    if (render.russianRouletteProb <= 0.0 || render.russianRouletteProb > 1.0) {
        std::cerr << "Error: Russian roulette probability must be in (0, 1]" << std::endl;
        return false;
    }
    
    if (camera.fov <= 0.0 || camera.fov >= 180.0) {
        std::cerr << "Error: Camera FOV must be in (0, 180)" << std::endl;
        return false;
    }
    
    return true;
}

void RenderConfig::PrintUsage() const {
    std::cout << "Usage: BickPathTracer [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --config <file>        Load configuration from JSON file\n";
    std::cout << "  --scene <file>         Scene configuration file\n";
    std::cout << "  --samples <count>      Number of samples per pixel\n";
    std::cout << "  --resolution <WxH>     Image resolution (e.g., 1024x768)\n";
    std::cout << "  --output <file>        Output image file path\n";
    std::cout << "  --threads <count>      Number of render threads (0 = auto)\n";
    std::cout << "  --debug                Enable debug mode\n";
    std::cout << "  --debug-images         Render debug images\n";
    std::cout << "  --debug-output <dir>   Debug output directory\n";
    std::cout << "  --help, -h             Show this help message\n\n";
    std::cout << "Examples:\n";
    std::cout << "  BickPathTracer --config configs/cornell_box.json\n";
    std::cout << "  BickPathTracer --scene configs/pbr_scene.json --samples 2048 --resolution 1024x1024\n";
    std::cout << "  BickPathTracer --config configs/debug.json --debug --debug-images\n";
}

bool RenderConfig::ParseResolution(const std::string& resolution) {
    size_t xPos = resolution.find('x');
    if (xPos == std::string::npos) {
        std::cerr << "Error: Invalid resolution format. Use WIDTHxHEIGHT (e.g., 1024x768)" << std::endl;
        return false;
    }
    
    try {
        render.imageWidth = std::stoi(resolution.substr(0, xPos));
        render.imageHeight = std::stoi(resolution.substr(xPos + 1));
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid resolution values" << std::endl;
        return false;
    }
}
