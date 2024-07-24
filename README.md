# 前言

这是一个纯手搓的软路径追踪渲染器,我叫它Bick.但是由于各种原因,没能实现一开始的技术目标,而且目前阶段也还是存在一些问题，渲染的结果也并不很正确。

# Feature

- [x] 软路径追踪
- [x] PBR
- [x] Metalli-Roughness材质
- [ ] 重要性采样
- [x] 纹理支持
- [x] obj文件支持
- [ ] BVH加速结构

# 渲染结果

## ConellBox

<img src = "result\ConellBox@1024ssp.png" alt = "ConellBox@1024ssp" >

## TextureSample

<img src = "result\Texture@1024ssp.png" alt = "TextureSample@1024ssp" >

<img src = "result\TextureAlbedo.png" alt = "Albedo">
Albedo

<img src = "result\TextureRoughness.png" alt = "Roughness">
Roughness

<img src = "result\TextureMetallic.png" alt = "Metallic">
Metallic

## PBRSample

<img src = "result\PBR@1024ssp.png" alt = "PBRSample@1024ssp" >>

## BlenderMonkey

<img src = "result\BlenderMonkey@8ssp.png" alt = "BlenderMonkey@8ssp" >

# 使用

1. 首先编译程序
2. 运行程序后会在根目录下生成一个ppm文件,该文件即为渲染结果
3. 打开ppm文件即可看到渲染结果
4. 可以使用[在线网站](https://www.ezyzip.com/cn-view-ppm.html)或安装相关插件来查看结果

Bick的绝大部分控制都依靠宏定义来完成.读者只需要找到Bick.h文件并更改其中的宏定义即可进行控制.

## 调整参数

1. SAMPLE_COUNT:SSP,调整采样次数,越高越精确,但渲染速度也越慢.
2. RUSSIAN_ROULETTE_PROBABILITY:俄罗斯轮盘赌概率,该值越大,更能模拟光线多次弹射
3. DEBUG_LOG_GAP:控制台输出光线路径的像素间隔,当打开控制台RayTrace功能时用到.

## 控制台输出

1. CONSOLE_OUTPUT_RENDER_PROGRESS:打开后,控制台会输出渲染进度
2. DEBUG_TRACERAY: 打开后,控制台会输出光线的路径,注意,打开后由于大量的输出,会极大减慢渲染速度.

## 切换场景

注意: 以下三个宏只能定义一个

1. SCENE_TEXTURE:对应第2张渲染图,该场景主要为了展示纹理功能
2. SCENE_CONELLBOX:对应第1张渲染图,该场景主要为了展示路径追踪的效果
3. SCENE_PBR:对应第3张渲染图,该场景主要为了展示PBR的效果
## 渲染模式

当定义以下任意一个宏时,将不会进行正常流程的渲染

1. RENDER_DEBUG_NORMAL: 打开后输出法线
2. RENDER_DEBUG_UV: 打开后输出UV
3. RENDER_DEBUG_HITPOINT:打开后输出交点位置
4. RENDER_DEBUG_DEPTH: 打开后输出深度
5. RENDER_DEBUG_RAYGEN: 打开后输出光线方向
6. RENDER_DEBUG_TRACE_RAY: 打开后输出光线路径(暂时无意义)
7. RENDER_DEBUG_SAMPLE: 打开后输出采样方向信息
8. RENDER_DEBUG_SHOW_ALBEDO: 打开后输出材质的漫反射率信息
9. RENDER_DEBUG_SHOW_ROUGHNESS: 打开后输出材质的粗糙度信息
10. RENDER_DEBUG_SHOW_METALLIC: 打开后输出材质的金属度信息


# 结语

事实上，我本人对现在的版本应该算是十分不满意.
首先可以肯定的是,一个多月前决定用自己甚至不算了解的C++来实现的决定,现在看来是完全低估了这里面的坑.
然后由于自己糟糕的开发习惯导致开发效率极低。
当然也有自己有将近一个月没有动过的原因。

其中最致命的莫过于糟糕的开发习惯。我习惯一路一直写下去一气呵成。在过于，虽然这也会给我带来困扰，但也不算难以解决。
在开发的前3天我就大约完成了大约70%的代码。这时的我还没有意识到我这糟糕的开发习惯将为我带来多大的困扰。

30倍，不夸张的说，我Debug花的时间起码是开发时间的30倍。
偏偏图形Debug本身就很有难度，任何细小的错误都会引起蝴蝶效应（我被此折磨太多次了）。
偏偏又由于不想依赖任何第三方库，Bick还不是实时的，等待的时间很容易就会让思路中断，让人烦躁。
然后我就拿出了我以前遇到这种情况时的惯用方法:重起高楼！

我带着这段时间的经验，创建了一个新的文件夹，在创建文件夹的那一刻，新的项目框架上该优化更改什么地方，文件结构要怎么变化，要构建一个cmake项目支持多平台，甚至还想好了要不名字就叫Bick吧....
口水情不自禁流出来的我飞快地开始敲出：int main ()
然后我再次使用我习惯的一鼓作气开发法，又舒畅地搭好了新Bick的框架。

问题多到我面对着十几小时的成果，还是毫不犹豫地丢进回收站，然后清空回收站。

在1个多月的debug过程中，我从一开始面对一副画面时纯靠猜哪里有问题，到现在摸到一点点图形debug的门路，虽然说不出什么东西来，但是我时切身地体会到了我思维的某处确实地得到了进化。

这个项目于我来说，最难的部分就是图形Debug的过程。
当然在开发的过程中也有遇到别的困难：什么语言问题，写C#从不管理内存导致的内存泄漏，编译链接调试还有各种稀奇古怪的问题...
我发誓我在用Unity开发时从来没有同时遇到过这么大量且奇葩多样的问题。

换而言之，这么多问题也让我确实学习成长了很多：我对c++的印象从总要过时的毫不优雅的语言变成了世界上最好的语言。我了解了如何开发一个开源项目（感觉Bick完全不够格啊）。我学到了很多编译相关的知识，我对IDE有了更深的理解，我对图形渲染尤其和主要是debug有了更多的了解，我在开发过程中学到了各种杂七杂八的知识......
当然其中最最重要我永远不会忘记的是：我的开发习惯有多差劲；以及理解和实现之间的鸿沟有多大，代码实战有多重要。

现在我认为合理的开发习惯应该是：一步一步开发，马上进行测试，不断优化和迭代到最佳状态，而非一步到位。

正如现在的Bick充斥着很多的HardCode和一些违反设计原则的东西以及屎堆代码。不过我下一步的计划依然不是重新另起高楼，而是先继续DEBUG至渲染结果正确无误。

不过再下一步的计划果然还是将我的Bick弃置于此吧。然后去用别的方案去实现一个路径追踪渲染器，毕竟这段时间里我了解到的知识多了不少。

# Reference
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
- [ZhiHu-Wang](https://zhuanlan.zhihu.com/p/370162390)

###### 发现这里面好像都没有关于图形debug的