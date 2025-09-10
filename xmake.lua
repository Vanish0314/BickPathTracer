-- xmake.lua for BickPathTracer (重构版本)
-- 现代化路径追踪渲染器项目配置

-- 设置项目基本信息
set_project("BickPathTracer")
set_version("2.0.0")  -- 重构版本

-- 设置C++标准
set_languages("cxx17")

-- 添加构建模式配置
add_rules("mode.debug", "mode.release")

-- 配置编译器选项
if is_mode("debug") then
    set_symbols("debug")
    set_optimize("none")
    add_defines("DEBUG")
elseif is_mode("release") then
    set_symbols("hidden")
    set_optimize("aggressive")
    add_defines("NDEBUG")
end

-- 添加编译选项
add_cxxflags("-Wall", "-Wextra")

-- 如果是GCC或Clang，添加多线程支持
if is_plat("linux", "macosx") then
    add_syslinks("pthread")
end

-- 主目标：重构后的现代版本
target("BickPathTracer")
    set_kind("binary")
    set_default(true)
    
    -- 添加源文件（递归搜索src目录）
    add_files("src/**.cpp")
    
    -- 添加头文件搜索路径
    add_includedirs("src", {public = true})
    add_includedirs("src/core", {public = true})
    add_includedirs("src/geometry", {public = true})
    add_includedirs("src/material", {public = true})
    add_includedirs("src/math", {public = true})
    add_includedirs("src/acceleration", {public = true})
    add_includedirs("src/utils", {public = true})
    add_includedirs("src/config", {public = true})
    
    -- 设置输出目录
    set_targetdir("build")
    
    -- 构建后拷贝资源
    after_build(function (target)
        print("正在拷贝资源文件...")
        
        -- 创建必要的目录
        os.mkdir(path.join(target:targetdir(), "assets"))
        os.mkdir(path.join(target:targetdir(), "assets/textures"))
        os.mkdir(path.join(target:targetdir(), "assets/meshes"))
        os.mkdir(path.join(target:targetdir(), "configs"))
        os.mkdir(path.join(target:targetdir(), "output"))
        os.mkdir(path.join(target:targetdir(), "output/images"))
        os.mkdir(path.join(target:targetdir(), "output/debug"))
        
        -- 拷贝资源文件到新结构
        if os.isdir("assets") then
            os.cp("assets/**", path.join(target:targetdir(), "assets"))
        end
        
        -- 旧资源目录已清理完毕
        
        -- 拷贝配置文件
        if os.isdir("configs") then
            os.cp("configs/*", path.join(target:targetdir(), "configs"))
        end
        
        print("资源文件拷贝完成")
        print("可执行文件: " .. path.join(target:targetdir(), target:name()))
        print("")
        print("使用示例:")
        print("  " .. path.join(target:targetdir(), target:name()) .. " --help")
        print("  " .. path.join(target:targetdir(), target:name()) .. " --config configs/default.json")
        print("  " .. path.join(target:targetdir(), target:name()) .. " --config configs/cornell_box.json")
    end)

-- 兼容目标已移除 - 旧代码遗产已清理完毕

-- 快速测试目标
target("BickPathTracer-Test")
    set_kind("binary")
    
    add_files("src/**.cpp")
    add_includedirs("src", {public = true})
    add_includedirs("src/core", {public = true})
    add_includedirs("src/geometry", {public = true})
    add_includedirs("src/material", {public = true})
    add_includedirs("src/math", {public = true})
    add_includedirs("src/acceleration", {public = true})
    add_includedirs("src/utils", {public = true})
    add_includedirs("src/config", {public = true})
    
    set_targetdir("build")
    
    -- 快速测试配置
    after_build(function (target)
        os.mkdir(path.join(target:targetdir(), "assets"))
        os.mkdir(path.join(target:targetdir(), "configs"))
        os.mkdir(path.join(target:targetdir(), "output/images"))
        
        if os.isdir("configs") then
            os.cp("configs/*", path.join(target:targetdir(), "configs"))
        end
    end)

-- 清理任务
task("clean-all")
    on_run(function ()
        os.rm("build")
        os.rm(".xmake")
        os.rm("*.ppm")
        os.rm("*.exe")
        print("已清理所有构建文件和输出图像")
    end)
    set_menu {
        usage = "xmake clean-all",
        description = "清理所有构建文件和输出图像"
    }

-- 运行任务
task("run")
    on_run(function ()
        os.exec("xmake build BickPathTracer")
        os.exec("xmake run BickPathTracer")
    end)
    set_menu {
        usage = "xmake run",
        description = "构建并运行默认场景"
    }

task("run-cornell")
    on_run(function ()
        os.exec("xmake build BickPathTracer")
        local exe_path = path.join("build", "BickPathTracer")
        os.exec(exe_path .. " --config configs/cornell_box.json")
    end)
    set_menu {
        usage = "xmake run-cornell",
        description = "构建并运行Cornell Box场景"
    }

task("run-pbr")
    on_run(function ()
        os.exec("xmake build BickPathTracer")
        local exe_path = path.join("build", "BickPathTracer")
        os.exec(exe_path .. " --config configs/default.json")
    end)
    set_menu {
        usage = "xmake run-pbr",
        description = "构建并运行PBR演示场景"
    }

task("run-debug")
    on_run(function ()
        os.exec("xmake build BickPathTracer")
        local exe_path = path.join("build", "BickPathTracer")
        os.exec(exe_path .. " --config configs/debug.json")
    end)
    set_menu {
        usage = "xmake run-debug",
        description = "构建并运行调试模式"
    }

task("quick-test")
    on_run(function ()
        os.exec("xmake build BickPathTracer")
        local exe_path = path.join("build", "BickPathTracer")
        os.exec(exe_path .. " --samples 16 --resolution 256x256 --output output/images/quick_test.ppm")
    end)
    set_menu {
        usage = "xmake quick-test",
        description = "快速测试渲染 (16采样, 256x256分辨率)"
    }

-- 构建模式任务
task("build-debug")
    on_run(function ()
        os.exec("xmake config --mode=debug")
        os.exec("xmake build BickPathTracer")
    end)
    set_menu {
        usage = "xmake build-debug",
        description = "调试模式构建"
    }

task("build-release")
    on_run(function ()
        os.exec("xmake config --mode=release")
        os.exec("xmake build BickPathTracer")
    end)
    set_menu {
        usage = "xmake build-release",
        description = "发布模式构建 (优化)"
    }

-- 帮助任务
task("help")
    on_run(function ()
        print("BickPathTracer - 重构版本 (已清理所有旧代码遗产)")
        print("可用的构建目标:")
        print("  BickPathTracer        - 主要目标 (现代化配置驱动版本)")
        print("  BickPathTracer-Test   - 测试目标")
        print("")
        print("可用的任务:")
        print("  xmake run             - 运行默认场景")
        print("  xmake run-cornell     - 运行Cornell Box场景")
        print("  xmake run-pbr         - 运行PBR演示场景") 
        print("  xmake run-debug       - 运行调试模式")
        print("  xmake quick-test      - 快速测试渲染")
        print("  xmake build-debug     - 调试模式构建")
        print("  xmake build-release   - 发布模式构建")
        print("  xmake clean-all       - 清理所有文件")
        print("")
        print("使用示例:")
        print("  xmake                 # 默认构建")
        print("  xmake run-cornell     # 运行Cornell Box场景")
        print("  xmake quick-test      # 快速测试")
    end)
    set_menu {
        usage = "xmake help",
        description = "显示帮助信息"
    }