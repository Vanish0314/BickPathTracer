/*
 * @Author: Vanish
 * @Date: 2024-09-03 20:07:11
 * @LastEditTime: 2024-09-03 20:09:43
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#include "Profiler.h"
#include <iostream>

Profiler::~Profiler() {
    auto duration = std::chrono::high_resolution_clock::now() - start;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    std::cout << "Profile \"" << name << "\": " << ms << "ms" << std::endl;
}
