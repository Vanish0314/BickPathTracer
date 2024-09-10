# 前言

这是一个纯手搓的软路径追踪渲染器,我叫它Bick.但是由于各种原因,没能实现一开始的技术目标,而且目前阶段也还是存在一些问题，渲染的结果也并不很正确。

# Feature

- [x] 软路径追踪
- [x] PBR
- [x] Metalli-Roughness材质
- [x] 光源重要性采样
- [ ] 多重重要性采样
- [x] 纹理支持
- [x] obj文件支持
- [x] 多线程
- [x] BVH加速结构
- [x] SAH算法

# 渲染结果

## ConellBox

<img src = "result\ConellBox.png" alt = "ConellBox@1024ssp" >


## PBRSample

<img src = "result\PBM.png" alt = "PBMSample@1024ssp" >>

## BVHSample

<img src = "result\dragon.png" alt = "dragon_871k@8ssp" >

# 使用

1. 首先编译程序
2. 运行程序后会在根目录下生成一个ppm文件,该文件即为渲染结果
3. 打开ppm文件即可看到渲染结果
4. 可以使用[在线网站](https://www.ezyzip.com/cn-view-ppm.html)或安装相关插件来查看结果

Bick的绝大部分控制都依靠宏定义来完成.读者只需要找到Bick.h文件并更改其中的宏定义即可进行控制.

<!-- # Reference
- [Games101](https://www.bilibili.com/video/BV1X7411F744/?spm_id_from=333.337.search-card.all.click)
- [PBRT](https://pbr-book.org/)
- [Games202](https://www.bilibili.com/video/BV1YK4y1T7yY/?spm_id_from=333.337.search-card.all.click)
- [Games104](https://www.bilibili.com/video/BV1oU4y1R7Km/?spm_id_from=333.337.search-card.all.click)
- [LearnOpenGL](https://learnopengl.com/)
- [OneWeekRayTracingSeries](https://raytracing.github.io/)
- [Fundamentals of Computer Graphics](http://repo.darmajaya.ac.id/5422/1/Fundamentals%20of%20Computer%20Graphics%2C%20Fourth%20Edition%20%28%20PDFDrive%20%29.pdf)
- [Whitted Ray Tracing](https://cglab.gist.ac.kr/courses/spring2019CG/2019_05_13_Whitted_Ray_Tracing.pdf)
- [Ray and Path Tracing](https://www.realtimerendering.com/erich/Ray_and_Path_Tracing_Today.pdf)
- [The Cherno - Ray Tracing Series](https://www.youtube.com/watch?v=gfW1Fhd9u9Q&list=PLlrATfBNZ98edc5GshdBtREv5asFW3yXl&ab_channel=TheCherno)
- [CodingAdvanture-RayTracing](https://www.youtube.com/watch?v=Qz0KTGYJtUk&t=454s&ab_channel=SebastianLague)
- [Wikipedia - Path tracing](https://en.wikipedia.org/wiki/Path_tracing)
- [ZhiHu-Mao](https://zhuanlan.zhihu.com/p/28059221)
- [ZhiHu-tkstar](https://zhuanlan.zhihu.com/p/146144853)
- [ZhiHu-Wang](https://zhuanlan.zhihu.com/p/370162390) -->
