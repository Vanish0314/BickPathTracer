/*
 * @Author: Vanish
 * @Date: 2024-06-01 21:38:49
 * @LastEditTime: 2024-07-14 15:26:12
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */


#include <iostream>
#include <chrono>

#include "Bick.h"

#include "Material.h"
#include "Sphere.h"
#include "Quad.h"
#include "Box.h"
#include "Mesh.h"
#include "Camera.h"
#include "Scene.h"

Scene* g_Scene;


int main() 
{
    std::cout<<"程序启动,开始执行...\n"<<std::endl;
    auto startTime = std::chrono::high_resolution_clock::now(); 

    std::cout<<"加载场景中...\n"<<std::endl;
#ifdef SCENE_TEXTURE
    /*****************创建贴图*******************/
    std::string PaintAlbedo  = "Texture\\PaintAbledo.ppm";
    std::string PaintRM      = "Texture\\PaintRM.ppm";
    auto        paint_albedo = std::make_shared<Texture>(PaintAlbedo);
    auto        paint_rm     = std::make_shared<Texture>(PaintRM);

    std::string WTF1_albedo_path = "Texture\\SuiBianHuaDeAlbedo.ppm";
    std::string WTF1_rm_path     = "Texture\\SuiBianHuaDeRm.ppm";
    auto        WTF1_albedo      = std::make_shared<Texture>(WTF1_albedo_path);
    auto        WTF1_rm          = std::make_shared<Texture>(WTF1_rm_path);

    std::string smilePath   = "Texture\\SmileAlbedo.ppm";
    std::string rmPath0      = "Texture\\1Rm.ppm";
    auto        smileAlbedo = std::make_shared<Texture>(smilePath);
    auto        smileRM     = std::make_shared<Texture>(smilePath);

    std::string sadPath   = "Texture\\SadAlbedo.ppm";
    std::string rmPath1    = "Texture\\1Rm.ppm";
    auto        sadAlbedo = std::make_shared<Texture>(sadPath);
    auto        sadRM     = std::make_shared<Texture>(rmPath1);
    /*****************创建材质*******************/

    auto paintMetal = std::make_shared<Material_PBM>(
        paint_albedo,
        paint_rm
    );
    auto WTF_Material1 = std::make_shared<Material_PBM>(
        WTF1_albedo,
        WTF1_rm
    );
    auto Smile_Material = std::make_shared<Material_PBM>(
        smileAlbedo,
        smileRM
    );
    auto Sad_Material = std::make_shared<Material_PBM>(
        sadAlbedo,
        sadRM
    );
    auto NormalWhite = std::make_shared<Material_PBM>(
        Color(1, 1, 1,1),
        1,
        0.1
    );
#endif
#ifdef SCENE_CONELLBOX
    auto white = std::make_shared<Material_PBM>(
        Color(1, 1, 1,1),
        1,
        0
    );
    auto RoughWhite = std::make_shared<Material_PBM>(
        Color(1, 1, 1,1),
        1,
        0.1
    );
    auto GlossyWhite = std::make_shared<Material_PBM>(
        Color(1, 1, 1,1),
        0.1,
        0.9
    );

    auto yellow = std::make_shared<Material_PBM>(
        Color(1, 1, 0,1),
        0.5,
        0.1
    );
    auto green = std::make_shared<Material_PBM>(
        Color(0, 1, 1,1),
        0.5,
        0.1
    );
    auto red = std::make_shared<Material_PBM>(
        Color(1, 0, 0,1),
        1,
        0
    );
    auto blue = std::make_shared<Material_PBM>(
        Color(0, 0, 1,1),
        1,
        0
    );
#endif
#ifdef SCENE_MESH
    auto MeshMaterial = std::make_shared<Material_PBM>(
        Color(1, 1, 1,1),
        1,
        0
    );
#endif

    auto white_Light = std::make_shared<Material_PBM>(
        Vector3(1,1,1),
        80000
    );
    auto monkey_Light = std::make_shared<Material_PBM>(
        Vector3(1,1,0),
        30000
    );
    auto banana_Light = std::make_shared<Material_PBM>(
        Vector3(0,1,1),
        80000
    );

    /*****************************************
     *                                       *
     *               场景                     *
     *                                       *
     *****************************************/
    //Cornell Box Sides

#ifdef SCENE_TEXTURE

    Quad* SmileWall = new Quad(
        "SmileWall",
        Vector3(5.55,0.0,0.0),
        Vector3(0,0,5.55),
        Vector3(0,5.55,0),
        Smile_Material
    );
    Quad* SadWall = new Quad(
        "SadWall",
        Vector3(0.0,0.0,5.55),
        Vector3(0,0,-5.55),
        Vector3(0,5.55,0),
        Sad_Material
    );
    Quad* NormalWhiteWall = new Quad(
        "上面的WhiteWall",
        Vector3(0.0,5.55,0.0),
        Vector3(5.55,0,0),
        Vector3(0,0,5.55),
        NormalWhite
    );
    Quad* WTF_Wall = new Quad(
        "下面的WhiteWall",
        Vector3(0,0,5.55),
        Vector3(5.55,0,0),
        Vector3(0,0,-5.55),
        WTF_Material1
    );
    Quad* PaintMetalWall = new Quad(
        "中间的WhiteWall",
        Vector3(5.55,0,5.55),
        Vector3(-5.55,0,0),
        Vector3(0,5.55,0),
        paintMetal
    );
#endif

#ifdef SCENE_CONELLBOX
        Quad* BlueWall = new Quad(
        "BlueWall",
        Vector3(5.5,0.0,0.0),
        Vector3(0,0,5.55),
        Vector3(0,5.55,0),
        blue
    );
    Quad* RedWall = new Quad(
        "RedWall",
        Vector3(0,0.0,0.0),
        Vector3(0,5.55,0),
        Vector3(0,0,5.55),
        red
    );
    Quad* WhiteWall = new Quad(
        "上面的WhiteWall",
        Vector3(0.0,5.55,0.0),
        Vector3(5.55,0,0),
        Vector3(0,0,5.55),
        white
    );
    Quad* WhiteWall2 = new Quad(
        "下面的WhiteWall",
        Vector3(0,0,0),
        Vector3(0,0,5.55),
        Vector3(5.55,0,0),
        white
    );
    Quad* WhiteWall3 = new Quad(
        "中间的WhiteWall",
        Vector3(0,0,5.55),
        Vector3(0,5.55,0),
        Vector3(5.55,0,0),
        white
    );
#endif
    
    /*****************************************
     *                                       *
     *               BOx                     *
     *                                       *
     *****************************************/
#ifdef SCENE_CONELLBOX
    std::vector<Quad*> box1 = Box(
        Vector3(1, 0, 3),
        Vector3(2, 2, 4),
        RoughWhite,
        "高粗箱子"
    );
    std::vector<Quad*> box2 = Box(
        Vector3(3.5, 0,2),
        Vector3(4.5,1 ,3),
        RoughWhite,
        "矮粗箱子"
    );
#endif
    
    /*****************************************
     *                                       *
     *               球体                     *
     *                                       *
     *****************************************/
#ifdef SCENE_PBR

    std::shared_ptr<Material_PBM> Mat[5][5];
    for(int i=0;i<5;i++){
        for(int j=0;j<5;j++){
            Mat[i][j] = std::make_shared<Material_PBM>(
                Color(1,0.2,0.3,1), 
                0.2 * i,
                0.2 * j
            );
        }
    }
    Sphere* spheres[5][5];
    for(int i=0;i<5;i++){
        for(int j=0;j<5;j++){
            spheres[i][j] = new Sphere(
                "Sphere"+std::to_string(i)+std::to_string(j),
                0.5,
                Vector3(1.5*i,1.5*j,0),
                Mat[i][j]
            );
        }
    }

    
#endif

#ifdef SCENE_MESH
    std::string meshPath = "Mesh\\BlenderMonkey.obj";

    Mesh* mesh = new Mesh(
        "BlenderMonkey",
        meshPath,
        Vector3(3,2,0),
        MeshMaterial
    );
#endif
    
    /*****************************************
     *                                       *
     *               灯光                     *
     *                                       *
     *****************************************/

    // add lights to scene
    Quad* Light = new Quad(
        "Light",
        Vector3(3.43+1.20,5.54,3.32 +1.20),
        Vector3(-3.80,0,0),
        Vector3(0,0,-3.80),
        white_Light
    );
    Quad* MonkeyLight = new Quad(
        "Light",
        Vector3(3.43+1.20,5.54,3.32 +1.20),
        Vector3(-3.80,0,0),
        Vector3(0,0,-3.80),
        monkey_Light
    );
    Quad* BananaLight = new Quad(
        "Light",
        Vector3(0,0,0),
        Vector3(0,0,5),
        Vector3(5,0,0),
        banana_Light
    );
    
    Sphere* SphereLight0 = new Sphere(
        "Light",
        2,
        Vector3(12,12,-10),
        white_Light
    );
    Sphere* SphereLight1 = new Sphere(
        "Light",
        2,
        Vector3(-2,12,-10),
        white_Light
    );
    Sphere* SphereLight2 = new Sphere(
        "Light",
        2,
        Vector3(12,-12,-10),
        white_Light
    );
    Sphere* SphereLight3 = new Sphere(
        "Light",
        2,
        Vector3(-12,-12,-10),
        white_Light
    );
    /*****************************************
     *                                       *
     *               添加到场景               *
     *                                       *
     *****************************************/

    // create scene
    g_Scene = new Scene();

#ifdef SCENE_CONELLBOX
    // add objects to scene
    g_Scene->AddObject(RedWall);
    g_Scene->AddObject(BlueWall);
    g_Scene->AddObject(WhiteWall); //上面的
    g_Scene->AddObject(WhiteWall2); //下面的
    g_Scene->AddObject(WhiteWall3); //中间的 
    //g_Scene->AddObject(GlassSphere);
    //g_Scene->AddObject(DiffuseSphere);
    g_Scene->AddObject(Light);

    g_Scene->AddObject(box1.at(0));
    g_Scene->AddObject(box1.at(1));
    g_Scene->AddObject(box1.at(2));
    g_Scene->AddObject(box1.at(3));
    g_Scene->AddObject(box1.at(4));
    g_Scene->AddObject(box1.at(5));
    g_Scene->AddObject(box2.at(0));
    g_Scene->AddObject(box2.at(1));
    g_Scene->AddObject(box2.at(2));
    g_Scene->AddObject(box2.at(3));
    g_Scene->AddObject(box2.at(4));
    g_Scene->AddObject(box2.at(5));
#endif
#ifdef SCENE_TEXTURE

     // add objects to scene
    g_Scene->AddObject(SmileWall);
    g_Scene->AddObject(SadWall);
    g_Scene->AddObject(NormalWhiteWall); //上面的
    g_Scene->AddObject(WTF_Wall); //下面的
    g_Scene->AddObject(PaintMetalWall); //中间的 
    g_Scene->AddObject(Light);
#endif
#ifdef SCENE_PBR
    g_Scene->AddObject(SphereLight0);
    g_Scene->AddObject(SphereLight1);
    g_Scene->AddObject(SphereLight2);
    g_Scene->AddObject(SphereLight3);
    for(int i=0;i<5;i++){
        for(int j=0;j<5;j++){
            g_Scene->AddObject(spheres[i][j]);
        }
    }
#endif
#ifdef SCENE_MESH
    g_Scene->AddObject(mesh);
    g_Scene->AddObject(MonkeyLight);
    g_Scene->AddObject(BananaLight);
#endif

#ifdef SCENE_PBR
    // set up camera
    Camera* camera = new Camera(
        Vector3(3, 3, -4),
        Vector3(  0,   0,    1), 
        Vector3(0.0, 1.0,  0.0),
        1024,
        1024,
        90
        );
#else
    Camera* camera = new Camera(
        Vector3(2.80, 2.50, -2.6),
        Vector3(  0,   0,    1), 
        Vector3(0.0, 1.0,  0.0),
        1024,
        1024,
        60
        );
#endif

    // creat ppm image file
    std::ofstream outputFile("RendenResult.ppm");
    
    std::cout<<"场景加载完成\n"<<std::endl;
    auto endSceneLoadingTime = std::chrono::high_resolution_clock::now();
    std::cout<<"场景加载耗时:"<<std::chrono::duration_cast<std::chrono::milliseconds>(endSceneLoadingTime - startTime).count()<<"ms\n"<<std::endl;

    std::cout<<"开始渲染...\n"<<std::endl;

    // render
    camera->Render(*g_Scene, outputFile);

    auto endTime = std::chrono::high_resolution_clock::now();
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    if(ms<1000){
        std::cout<<"渲染耗时:"<<ms<<"ms\n"<<std::endl;
    }
    else if(ms<1000 * 60){
        std::cout<<"渲染耗时:"<<ms/1000.0<<"s\n"<<std::endl;
    }
    else if(ms<1000 * 60 * 60){
        std::cout<<"渲染耗时:"<<ms/1000/60<<"min"<<ms/1000%60<<"s\n"<<std::endl;
    }
    else{
        std::cout<<"渲染耗时:"<<ms/1000/60/60<<"h"<<ms/1000/60%60<<"min"<<ms/1000%60<<"s\n"<<std::endl;
    }

    // close file
    outputFile.close(); 

    // 释放内存
#ifdef SCENE_TEXTURE
    delete SmileWall;
    delete SadWall;
    delete NormalWhiteWall;
    delete WTF_Wall;
    delete PaintMetalWall;
#endif

#ifdef SCENE_CONELLBOX
    delete RedWall;
    delete BlueWall;
    delete WhiteWall;
    delete WhiteWall2;
    delete WhiteWall3;
#endif

    delete Light;
    delete camera;
    delete g_Scene;

    return 0;
}

