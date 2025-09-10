#!/bin/bash

# BickPathTracer 构建脚本 (重构版本)
# 支持 xmake 和 CMake 双构建系统

set -e  # 遇到错误时退出

# 获取脚本所在目录的上级目录作为项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

# 默认使用 xmake
BUILD_SYSTEM="xmake"

# 解析参数
while [[ $# -gt 0 ]]; do
    case $1 in
        --cmake)
            BUILD_SYSTEM="cmake"
            shift
            ;;
        --xmake)
            BUILD_SYSTEM="xmake"
            shift
            ;;
        debug|release)
            BUILD_TYPE="$1"
            shift
            ;;
        -h|--help)
            echo "BickPathTracer 构建脚本"
            echo ""
            echo "用法: $0 [选项] [构建类型]"
            echo ""
            echo "选项:"
            echo "  --xmake    使用 xmake 构建系统 (默认)"
            echo "  --cmake    使用 CMake 构建系统"
            echo "  -h,--help  显示帮助信息"
            echo ""
            echo "构建类型:"
            echo "  debug      调试构建"
            echo "  release    发布构建 (默认)"
            echo ""
            echo "示例:"
            echo "  $0                    # 使用 xmake 发布构建"
            echo "  $0 debug              # 使用 xmake 调试构建"
            echo "  $0 --cmake release    # 使用 CMake 发布构建"
            echo "  $0 --xmake debug      # 使用 xmake 调试构建"
            exit 0
            ;;
        *)
            echo "未知参数: $1"
            echo "使用 $0 --help 查看帮助"
            exit 1
            ;;
    esac
done

# 设置默认构建类型
if [ -z "$BUILD_TYPE" ]; then
    BUILD_TYPE="release"
fi

echo "========================================"
echo "BickPathTracer 重构版本构建脚本"
echo "构建系统: $BUILD_SYSTEM"
echo "构建类型: $BUILD_TYPE"
echo "项目根目录: $PROJECT_ROOT"
echo "========================================"

if [ "$BUILD_SYSTEM" = "xmake" ]; then
    # 检查 xmake 是否已安装
    if ! command -v xmake &> /dev/null; then
        echo "错误: xmake 未安装"
        echo "请访问 https://xmake.io 安装 xmake"
        exit 1
    fi
    
    echo "使用 xmake 构建..."
    
    # 配置构建模式
    echo "配置构建模式: $BUILD_TYPE"
    xmake config --mode="$BUILD_TYPE"
    
    # 构建项目
    echo "构建项目..."
    xmake build BickPathTracer
    
    # 检查构建是否成功
    if [ $? -eq 0 ]; then
        echo "========================================"
        echo "xmake 构建成功!"
        echo "可执行文件: $PROJECT_ROOT/build/BickPathTracer"
        echo ""
        echo "运行示例:"
        echo "  cd $PROJECT_ROOT"
        echo "  ./build/BickPathTracer --help"
        echo "  ./build/BickPathTracer --config configs/cornell_box.json"
        echo "  ./build/BickPathTracer --samples 64 --resolution 512x512"
        echo ""
        echo "或者使用 xmake 任务:"
        echo "  xmake run              # 运行默认场景"
        echo "  xmake run-cornell      # 运行 Cornell Box"
        echo "  xmake quick-test       # 快速测试"
        echo "  xmake help             # 查看所有可用任务"
        echo "========================================"
    else
        echo "xmake 构建失败!"
        exit 1
    fi
    
elif [ "$BUILD_SYSTEM" = "cmake" ]; then
    # 检查 CMake 是否已安装
    if ! command -v cmake &> /dev/null; then
        echo "错误: CMake 未安装"
        echo "请安装 CMake 后重试"
        exit 1
    fi
    
    echo "使用 CMake 构建..."
    
    # 设置 CMake 构建类型
    if [ "$BUILD_TYPE" = "debug" ]; then
        CMAKE_BUILD_TYPE="Debug"
    else
        CMAKE_BUILD_TYPE="Release"
    fi
    
    # 创建构建目录
    BUILD_DIR="build_cmake"
    if [ ! -d "$BUILD_DIR" ]; then
        echo "创建构建目录: $BUILD_DIR"
        mkdir -p "$BUILD_DIR"
    fi
    
    cd "$BUILD_DIR"
    
    # 运行CMake配置
    echo "配置CMake..."
    cmake .. -DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE"
    
    # 编译项目
    echo "编译项目..."
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    
    # 检查编译是否成功
    if [ $? -eq 0 ]; then
        echo "========================================"
        echo "CMake 构建成功!"
        echo "可执行文件: $PROJECT_ROOT/build/BickPathTracer"
        echo ""
        echo "运行示例:"
        echo "  cd $PROJECT_ROOT"
        echo "  ./build/BickPathTracer --config configs/default.json"
        echo "  ./build/BickPathTracer --config configs/cornell_box.json"
        echo "  ./build/BickPathTracer --config configs/debug.json"
        echo "========================================"
    else
        echo "CMake 构建失败!"
        exit 1
    fi
fi