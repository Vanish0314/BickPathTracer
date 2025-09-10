/*
 * @Author: Vanish
 * @Date: 2025-09-10 22:15:00
 * @LastEditTime: 2025-09-10 22:15:00
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */

#include "Debuger.h"
#include "../core/Scene.h"
#include "../math/Vector3.h"
#include "../core/Ray.h"

// 现代化的调试器实现
// 移除了所有愚蠢的宏控制，使用配置驱动的调试系统

// 移除了构造函数 - Debuger现在是静态类

bool Debuger::DebugMode() {
    // 调试模式现在通过配置系统控制，而不是编译时宏
    return false; // 默认关闭，应该从RenderConfig读取
}

Vector3 Debuger::DebugResult(Ray& /* ray */) {
    // 简化的调试函数 - 具体的调试功能现在通过RenderConfig控制
    // 不再使用愚蠢的编译时宏定义
    
    // 如果需要调试信息，应该通过配置系统启用
    // 例如: config.render.debugMode && config.render.renderDebugImages
    
    return Vector3(0, 0, 0); // 默认返回值
}

// 所有旧的宏控制调试代码已被移除
// 新的调试系统应该：
// 1. 使用运行时配置而不是编译时宏
// 2. 通过RenderConfig.render.debugMode控制
// 3. 输出到统一的调试目录
// 4. 使用现代的日志系统