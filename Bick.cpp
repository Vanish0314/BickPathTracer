/*
 * @Author: Vanish
 * @Date: 2024-06-01 21:38:49
 * @LastEditTime: 2024-09-10 02:45:24
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */


#include <iostream>
#include <chrono>

#include "Bick.h"

#include "Material.h"
#include "Profiler.h"
#include "Sphere.h"
#include "Quad.h"
#include "Box.h"
#include "Mesh.h"
#include "Camera.h"
#include "Film.h"
#include "ThreadPool.h"
#include "Scene.h"

#include "3rd/imgui/imgui.h"

Vector3 Shade(Vector3 p , Vector3 wo,Vector3 normal,double t,std::shared_ptr<Material_PBM> material)
{

    if(material->isEmissive)
        return material->EmissiveTerm(wo,p)*Vector3::Dot(wo,normal)/RUSSIAN_ROULETTE_PROBABILITY;

    Vector3 dir_light = Vector3(0,0,0);
    Vector3 indir_light = Vector3(0,0,0);

#ifdef RENDER_MODE_IS
    for(auto light : g_Scene->lights)
    {
        SampleResult sample = light->UnitSamplePdf();

        Ray occlulusionRay(
            p + normal * 0.01,
            (sample.position-p).Normalized()
        );
        HitRecord hitRecord;
        g_Scene->Hit(occlulusionRay,Interval(0,Vector3::Distance(p,sample.position)-0.01),hitRecord);
        if(!hitRecord.hitted)
        {
            auto m   = light->material;
            auto pbm = std::dynamic_pointer_cast<Material_PBM>(m);

            Vector3 emi       = pbm->EmissiveTerm(Vector3(0,0,0)-occlulusionRay.direction,sample.position);
            Vector3 fr        = material->BRDF(p,wo,occlulusionRay.direction,normal);
            double  cos1      = std::max(0.0,Vector3::Dot(occlulusionRay.direction.Normalized(),normal));
            //double  cos2      = std::max(0.0,Vector3::Dot((p-sample.position).Normalized(),sample.normal));
            double  delta     = Vector3::Distance(p,sample.position);
            double  term      = cos1/std::pow(delta,2);
            double  pdf       = sample.pdf;
            
            dir_light = emi * fr * term / pdf;
        }
    }

    
#endif

    if(Random::GetRandomDouble(0,1)>RUSSIAN_ROULETTE_PROBABILITY)
        return dir_light;

    Ray ray(Vector3(0,0,0),Vector3(0,0,0));
    double pdf = PDF::SampleHemisphere(p, ray,normal);
    Vector3 wi = ray.direction;

    HitRecord bounceRecord;
    g_Scene->Hit(ray,Interval(),bounceRecord);
    if(!bounceRecord.hitted)
        return dir_light;

    auto mHitted   = bounceRecord.material;
    auto pbmHitted = std::dynamic_pointer_cast<Material_PBM>(mHitted);
    
    double cosTheta = std::max(0.0, Vector3::Dot(wi, normal));

    if(!pbmHitted->isEmissive)
    {
        Vector3 li          = Shade(bounceRecord.hitPoint,Vector3(0,0,0)-wi,bounceRecord.normal,bounceRecord.t,pbmHitted);
        Vector3 fr          = material->BRDF(p,wo,wi,normal,bounceRecord.u,bounceRecord.v);
        Vector3 result      = li * fr * cosTheta / pdf/RUSSIAN_ROULETTE_PROBABILITY;
                indir_light = result;
    }   
#ifndef RENDER_MODE_IS
    else
    {
        Vector3 emi    = pbmHitted->EmissiveTerm(Vector3(0,0,0)-wi,p);
        Vector3 fr     = material->BRDF(p,wo,wi,normal,bounceRecord.u,bounceRecord.v);
        Vector3 result = emi * fr * cosTheta / pdf/RUSSIAN_ROULETTE_PROBABILITY;
        return result;
    }
#endif
    return dir_light + indir_light;
}

Vector3 Shade(Vector3 p , Vector3 wo,Vector3 normal,double t,std::shared_ptr<Material_PBM> material,Film &debug_film,int x = 0,int y = 0,int depth = 0)
{
    if(material->isEmissive)
        return material->EmissiveTerm(wo,p)*Vector3::Dot(wo,normal)/RUSSIAN_ROULETTE_PROBABILITY;

    Vector3 dir_light = Vector3(0,0,0);
    Vector3 indir_light = Vector3(0,0,0);

#ifdef RENDER_MODE_IS
    for(auto light : g_Scene->lights)
    {
        SampleResult sample = light->UnitSamplePdf();

        Ray occlulusionRay(
            p + normal * 0.01,
            (sample.position-p).Normalized()
        );
        HitRecord hitRecord;
        g_Scene->Hit(occlulusionRay,Interval(0,Vector3::Distance(p,sample.position)-0.01),hitRecord);
        if(!hitRecord.hitted)
        {
            auto m   = light->material;
            auto pbm = std::dynamic_pointer_cast<Material_PBM>(m);

            Vector3 emi       = pbm->EmissiveTerm(Vector3(0,0,0)-occlulusionRay.direction,sample.position);
            Vector3 fr        = material->BRDF(p,wo,occlulusionRay.direction,normal);
            double  cos1      = std::max(0.0,Vector3::Dot(occlulusionRay.direction.Normalized(),normal));
            double  cos2      = std::max(0.0,Vector3::Dot((p-sample.position).Normalized(),sample.normal));
            double  delta     = Vector3::Distance(p,sample.position);
            double  term      = (cos1)/std::pow(delta,2);
            double  pdf       = sample.pdf;
            
            dir_light = emi * fr * term / pdf;
        }
    }

    
#endif

    if(Random::GetRandomDouble(0,1)>RUSSIAN_ROULETTE_PROBABILITY)
        return dir_light;

    Ray ray(Vector3(0,0,0),Vector3(0,0,0));
    double pdf = PDF::SampleHemisphere(p, ray,normal);
    Vector3 wi = ray.direction;

    HitRecord hitRecord;
    g_Scene->Hit(ray,Interval(),hitRecord);
    if(!hitRecord.hitted)
        return dir_light;


    auto mHitted   = hitRecord.material;
    auto pbmHitted = std::dynamic_pointer_cast<Material_PBM>(mHitted);
    
    double cosTheta = std::max(0.0, Vector3::Dot(wi, normal));

    if(!pbmHitted->isEmissive)
    {
        Vector3 li          = Shade(hitRecord.hitPoint,Vector3(0,0,0)-wi,hitRecord.normal,hitRecord.t,pbmHitted,debug_film,x,y,depth+1);
        Vector3 fr          = material->BRDF(p,wo,wi,normal,hitRecord.u,hitRecord.v);
        Vector3 result      = li * fr * cosTheta / pdf/RUSSIAN_ROULETTE_PROBABILITY;
                indir_light = result;

        debug_film.AddSample(x,y,li * 3);
        // if(debug_film.HasFilm("ShaderDebug/DiffuseLi-" + depth))
        // {
        //     debug_film.SetFilm("ShaderDebug/DiffuseLi-" + depth,x,y,(li.Normalized() + Vector3(1,1,1))/2);
        // }
        // else
        // {
        //     debug_film.AddFilm("ShaderDebug/DiffuseLi-" + depth);
        //     debug_film.SetFilm("ShaderDebug/DiffuseLi-" + depth,x,y,(li.Normalized() + Vector3(1,1,1))/2);
        // }

        // if(debug_film.HasFilm("ShaderDebug/ray.direction-" + depth))
        // {
        //     debug_film.SetFilm("ShaderDebug/ray.direction-" + depth,x,y,(wi.Normalized() + Vector3(1,1,1))/2);
        // }
        // else
        // {
        //     debug_film.AddFilm("ShaderDebug/ray.direction-" + depth);
        //     debug_film.SetFilm("ShaderDebug/ray.direction-" + depth,x,y,(wi.Normalized() + Vector3(1,1,1))/2);
        // }

        // if(debug_film.HasFilm("ShaderDebug/Hitted-" + depth))
        // {
        //     debug_film.SetFilm("ShaderDebug/Hitted-" + depth,x,y,hitRecord.hitted ? Vector3(255,255,255) : Vector3(0,0,0));
        // }
        // else
        // {
        //     debug_film.AddFilm("ShaderDebug/Hitted-" + depth);
        //     debug_film.SetFilm("ShaderDebug/Hitted-" + depth,x,y,hitRecord.hitted ? Vector3(255,255,255) : Vector3(0,0,0));
        // }

        // if(debug_film.HasFilm("ShaderDebug/inDirLight-" + depth))
        // {
        //     debug_film.SetFilm("ShaderDebug/inDirLight-" + depth,x,y,(indir_light * 2));
        // }
        // else
        // {
        //     debug_film.AddFilm("ShaderDebug/inDirLight-" + depth);
        //     debug_film.SetFilm("ShaderDebug/inDirLight-" + depth,x,y,(indir_light * 2));
        // }




    }   
#ifndef RENDER_MODE_IS
    else
    {
        Vector3 emi    = pbmHitted->EmissiveTerm(Vector3(0,0,0)-wi,p);
        Vector3 fr     = material->BRDF(p,wo,wi,normal,hitRecord.u,hitRecord.v);
        Vector3 result = emi * fr * cosTheta / pdf/RUSSIAN_ROULETTE_PROBABILITY;
        return result;
    }
#endif
    return dir_light + indir_light;
}
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
        0.9,
        0.4
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
        0.5,
        0.5
    );
#endif

    auto white_Light = std::make_shared<Material_PBM>(
        Vector3(1,1,1),
        80
    );
    auto monkey_Light = std::make_shared<Material_PBM>(
        Vector3(0,1,1),
        3
    );
    auto banana_Light = std::make_shared<Material_PBM>(
        Vector3(1,0.17,1),
        3
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
    Sphere* BlueSphere = new Sphere(
        "BlueSphere",
        10000,
        Vector3(5.55+10000,5.55/2,5.55/2),
        blue
    );
    Quad* RedWall = new Quad(
        "RedWall",
        Vector3(0,0.0,0.0),
        Vector3(0,5.55,0),
        Vector3(0,0,5.55),
        red
    );
    Sphere* RedSphere = new Sphere(
        "RedSphere",
        10000,
        Vector3(-10000,5.55/2,5.55/2),
        red
    );
    Quad* WhiteWall = new Quad(
        "上面的WhiteWall",
        Vector3(0,5.55,0),
        Vector3(5.55,0,0),
        Vector3(0,0,5.55),
        white
    );
    Sphere* UpSphere = new Sphere(
        "UpSphere",
        10000,
        Vector3(0,5.55 + 10000,0),
        white
    );
    Quad* WhiteWall2 = new Quad(
        "下面的WhiteWall",
        Vector3(0,0,0),
        Vector3(0,0,5.55),
        Vector3(5.55,0,0),
        white
    );
    Sphere* DownSphere = new Sphere(
        "DownSphere",
        10000,
        Vector3(0, - 10000,0),
        white
    );
    Quad* WhiteWall3 = new Quad(
        "中间的WhiteWall",
        Vector3(0,0,5.55),
        Vector3(0,5.55,0),
        Vector3(5.55,0,0),
        white
    );
    Sphere* midSphere = new Sphere(
        "midSphere",
        10000,
        Vector3(5.55/2,5.55/2,10000 + 5.55),
        white
    );
    Quad* CameraWall = new Quad(
        "CameraWall",
        Vector3(0,0,0),
        Vector3(5.55*2,0,0),
        Vector3(0,5.55*2,0),
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
        Vector3(0.7, 0, 3),
        Vector3(1, 0, -0.3),
        Vector3(0.3,0,1),
        Vector3(0,2,0),
        white,
        "高粗箱子"
    );
    std::vector<Quad*> box2 = Box(
        Vector3(3.5, 0,2),
        Vector3(4.5,1 ,3),
        RoughWhite,
        "矮粗箱子"
    );
    Sphere* MetalSpere = new Sphere(
        "glassSphere",
        0.5,
        Vector3(4,0.5,2),
        GlossyWhite
    );

    Sphere* rightSphere = new Sphere(
        "rightSphere",
        1,
        Vector3(2, 0, 2.3),
        white
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
    std::string meshPath = "Mesh\\dragon_87k.obj";

    Mesh* mesh = new Mesh(
        "dragon",
        meshPath,
        Vector3(2.8,2.42,-1.6),
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
        Vector3(5.55/4,5.54,5.55/3),
        Vector3(5.55/2,0,0),
        Vector3(0,0,5.55/2),
        white_Light
    );
    Sphere* ConellSphereLight = new Sphere(
        "ConellSphereLight",
        0.2,
        Vector3(5.55/2,5.55/2,5.55/2),
        white_Light
    );

    Quad* MonkeyLight = new Quad(
        "Light",
        Vector3(-10000,8,-10000),
        Vector3(20000,0,0),
        Vector3(0,0,20000),
        monkey_Light
    );
    Quad* BananaLight = new Quad(
        "Light",
        Vector3(-10000,0,-10000),
        Vector3(0,0,20000),
        Vector3(20000,0,0),
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
    g_Scene->AddObject(BlueSphere);
    g_Scene->AddObject(RedSphere);
    g_Scene->AddObject(CameraWall);
    //g_Scene->AddObject(BlueWal);
    //g_Scene->AddObject(RedWall);
    //g_Scene->AddObject(UpSphere);
    g_Scene->AddObject(midSphere); //中间的
    g_Scene->AddObject(DownSphere);
    g_Scene->AddObject(WhiteWall); //上面的
    //g_Scene->AddObject(WhiteWall2); //下面的
    //g_Scene->AddObject(WhiteWall3); //中间的
    //g_Scene->AddObject(GlassSphere);
    //g_Scene->AddObject(DiffuseSphere);
    //g_Scene->AddObject(Light);
    g_Scene->AddObject(ConellSphereLight);
    g_Scene->AddObject(MetalSpere);
    
    g_Scene->AddObject(box1.at(0));
    g_Scene->AddObject(box1.at(1));
    g_Scene->AddObject(box1.at(2));
    g_Scene->AddObject(box1.at(3));
    g_Scene->AddObject(box1.at(4));
    g_Scene->AddObject(box1.at(5));
    // g_Scene->AddObject(box2.at(0));
    // g_Scene->AddObject(box2.at(1));
    // g_Scene->AddObject(box2.at(2));
    // g_Scene->AddObject(box2.at(3));
    // g_Scene->AddObject(box2.at(4));
    // g_Scene->AddObject(box2.at(5));
#endif
#ifdef SCENE_TEXTURE

     // add objects to scene
    g_Scene->AddObject(SmileWall);
    g_Scene->AddObject(SadWall);
    g_Scene->AddObject(NormalWhiteWall); //上面的
    g_Scene->AddObject(WTF_Wall); //下面的
    g_Scene->AddObject(PaintMetalWall); //中间的 
    g_Scene->AddObject(ConellSphereLight);
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
        512,
        512,
        60
        );
#endif
    
    #ifdef RENDER_DEBUG_IMAGE
    std::cout<<"开始渲染DebugImage\n";
    Film normal         (camera->imageWidth, camera->imageHeight);
    Film uv             (camera->imageWidth, camera->imageHeight);
    Film hitPoint       (camera->imageWidth, camera->imageHeight);
    Film depth          (camera->imageWidth, camera->imageHeight);
    Film directLight    (camera->imageWidth, camera->imageHeight);
    Film ISDirectLight  (camera->imageWidth, camera->imageHeight);
    Film directLightDir (camera->imageWidth, camera->imageHeight);
    Film directLitted   (camera->imageWidth, camera->imageHeight);
    Film viewDirection  (camera->imageWidth, camera->imageHeight);
    Film cos1Film       (camera->imageWidth, camera->imageHeight);
    Film cos2Film       (camera->imageWidth, camera->imageHeight);
    Film brdfFilm       (camera->imageWidth, camera->imageHeight);
    Film deltaFilm      (camera->imageWidth, camera->imageHeight);
    Film termFilm       (camera->imageWidth, camera->imageHeight);
    Film albedo         (camera->imageWidth, camera->imageHeight);
    Film roughness      (camera->imageWidth, camera->imageHeight);
    Film metallic       (camera->imageWidth, camera->imageHeight);
    Film reflection     (camera->imageWidth, camera->imageHeight);
    Film indirectLight  (camera->imageWidth, camera->imageHeight);

    ThreadPool debugPool;
    debugPool.ParallelFor(camera->imageWidth,camera->imageHeight,[&](int i,int j){\
        Ray ray = Ray(Vector3(0, 0, 0), Vector3(0, 0, 0));
        camera->GetRay(i, j, ray);
        HitRecord hitRecord;
        g_Scene->Hit(ray,Interval(),hitRecord);

        //normal
        normal.AddSample(i,j,Vector3(
            (hitRecord.normal.x+1)/2,
            (hitRecord.normal.y+1)/2,
            (hitRecord.normal.z+1)/2
        )*255);
        
        //uv
        uv.AddSample(i,j,Vector3(hitRecord.u,hitRecord.v,0)*255);

        //fr
        auto m = hitRecord.material;
        auto pbm = std::dynamic_pointer_cast<Material_PBM>(m);
        Vector3 fr = pbm->BRDF(hitRecord.hitPoint,Vector3(0,0,0) - ray.direction,g_Scene->lights[0]->position-hitRecord.hitPoint,hitRecord.normal);
        brdfFilm.AddSample(i,j,fr*255);
        
        //depth
        depth.AddSample(i,j,Vector3(1,1,1)*hitRecord.t*12);

        //DirectLight
        Vector3 beta = Vector3(0, 0, 0);
        for(auto light : g_Scene->lights)
        {
            auto m = light->material;
            auto pbm = std::dynamic_pointer_cast<Material_PBM>(m);

            Vector3 L_dir = light->position - hitRecord.hitPoint;
            L_dir.Normalize();
            hitPoint.AddSample(i,j,(hitRecord.hitPoint.Normalized()+Vector3(1,1,1))/2*255);
            directLightDir.AddSample(i,j,(L_dir+Vector3(1,1,1))/2 *255);

            Vector3 viewDir = Vector3(0, 0, 0) - ray.direction;
            viewDir.Normalize();
            viewDirection.AddSample(i,j,(viewDir+Vector3(1,1,1)/2)*255);
            
            Vector3 halfwayDir = (L_dir + viewDir).Normalized();

            double spec = std::pow(std::max(Vector3::Dot(halfwayDir, hitRecord.normal), 0.0), 1);
            beta = pbm->EmissiveTerm(hitRecord.hitPoint,hitRecord.hitPoint) * spec;



            SampleResult sample = light->UnitSamplePdf();
            
            Ray occlulusionRay(
                hitRecord.hitPoint + hitRecord.normal * 0.01,
                (sample.position-hitRecord.hitPoint).Normalized()
            );
            HitRecord OccHitRecord;
            g_Scene->Hit(occlulusionRay,Interval(0,Vector3::Distance(hitRecord.hitPoint,sample.position)-0.1),OccHitRecord);

            if(!OccHitRecord.hitted)
                directLitted.AddSample(i,j,Vector3(1,1,1)*255);
            double  cos1      = Vector3::Dot(L_dir,hitRecord.normal);//问题所在
            cos1Film.AddSample(i,j,Vector3(cos1,cos1,cos1)*255);
            double  cos2      = Vector3::Dot(sample.normal,occlulusionRay.direction);
            cos2Film.AddSample(i,j,Vector3(cos2,cos2,cos2)*255);
            double  delta     = Vector3::Distance(hitRecord.hitPoint,light->position) - 0.5;
            deltaFilm.AddSample(i,j,Vector3(1/std::pow(delta,2),1/std::pow(delta,2),1/std::pow(delta,2))*255);
            double  term      = cos1/std::pow(delta,2);
            termFilm.AddSample(i,j,Vector3(term,term,term)*255);

            Vector3 emi = std::dynamic_pointer_cast<Material_PBM>(light->material)->EmissiveTerm(hitRecord.hitPoint,sample.position);
            ISDirectLight.AddSample(i,j,emi*fr*term/sample.pdf);

        }
        directLight.AddSample(i,j,beta/60);

        //directLightDir


        // //albedo
        // auto m = hitRecord.material;
        // //转换m为Material_PBM
        // auto pbm = dynamic_cast<Material_PBM*>(m.get());
        // pbm->UpdateBuffer(hitRecord.u,hitRecord.v);
        // Color color = pbm->albedoBuffer;
        // albedo.AddSample(i,j,Vector3(color.r,color.g,color.b)*255);

        // //roughness
        // //metallic

        // //reflection
        // Ray reflect = Ray(
        //     ray.at(hitRecord.t),
        //     Vector3::Reflect(ray.direction, hitRecord.normal)
        // );
        // HitRecord reflectRecord;
        // g_Scene->Hit(reflect, Interval(), reflectRecord);
        // if (reflectRecord.hitted)
        // {
        //     auto m = reflectRecord.material;
        //     auto pbm = std::dynamic_pointer_cast<Material_PBM>(m);
        //     pbm->UpdateBuffer(reflectRecord.u, reflectRecord.v);
        //     Color color = pbm->albedoBuffer;
        //     reflection.AddSample(i,j,Vector3(color.r,color.g,color.b)*255);
        // }
        // else
        // {
        //     reflection.AddSample(i,j,Vector3(0,0,0));
        // }
    });

    debugPool.WaitAll();
    normal.          Save_P3_WithoutCorrection("Debug/normal.ppm");
    uv.              Save_P3_WithoutCorrection("Debug/uv.ppm");
    hitPoint.        Save_P3_WithoutCorrection("Debug/hitPoint.ppm");
    depth.           Save_P3_WithoutCorrection("Debug/depth.ppm");
    directLight.     Save_P3_WithoutCorrection("Debug/directLight.ppm");
    ISDirectLight.   Save_P3                  ("Debug/ISDirectLight.ppm");
    directLightDir.  Save_P3_WithoutCorrection("Debug/directLightDir.ppm");
    directLitted.    Save_P3_WithoutCorrection("Debug/directLitted.ppm");
    viewDirection.   Save_P3_WithoutCorrection("Debug/viewDirection.ppm");
    cos1Film.        Save_P3_WithoutCorrection("Debug/cos1Film.ppm");
    cos2Film.        Save_P3_WithoutCorrection("Debug/cos2Film.ppm");
    brdfFilm.        Save_P3_WithoutCorrection("Debug/brdfFilm.ppm");
    deltaFilm.       Save_P3_WithoutCorrection("Debug/deltaFilm.ppm");
    termFilm.        Save_P3_WithoutCorrection("Debug/termFilm.ppm");
    albedo.          Save_P3_WithoutCorrection("Debug/albedo.ppm");
    roughness.       Save_P3_WithoutCorrection("Debug/roughness.ppm");
    metallic.        Save_P3_WithoutCorrection("Debug/metallic.ppm");
    reflection.      Save_P3_WithoutCorrection("Debug/reflection.ppm");

    std::cout<<"Debug图片保存完成\n"<<std::endl;

    #endif
    
    std::cout<<"场景加载完成\n"<<std::endl;
    auto endSceneLoadingTime = std::chrono::high_resolution_clock::now();
    std::cout<<"场景加载耗时:"<<std::chrono::duration_cast<std::chrono::milliseconds>(endSceneLoadingTime - startTime).count()<<"ms\n"<<std::endl;
    std::cout<<"开始渲染...\n"<<std::endl;

    // rendering
    Film film(camera->imageWidth, camera->imageHeight);
    Film singleDebugFilm(camera->imageWidth, camera->imageHeight);
    DebugFilm debugFilm(camera->imageWidth, camera->imageHeight);
    ThreadPool threadPool;
    int currentSSP = 0;
    int increaseStep = 128;

    Vector3 maxLi = Vector3(0, 0, 0);
    for(auto light : g_Scene->lights)
    {
        maxLi += light->material->emissiveDistribution * light->material->emissiveIntensity;
    }

    while (currentSSP < SAMPLE_COUNT)
    {
        threadPool.ParallelFor(camera->imageWidth, camera->imageHeight, [&](int i, int j)
        {
            for (int s = 0; s < increaseStep; s++)
            {
                if(currentSSP >= SAMPLE_COUNT) break;
                
                Vector3 radiance = Vector3(0, 0, 0);
                Ray ray = Ray(Vector3(0, 0, 0), Vector3(0, 0, 0));
                camera->GetRay(i, j, ray);
                HitRecord hitRecord;
                g_Scene->Hit(ray, Interval(), hitRecord);
                if (!hitRecord.hitted)
                    break;
                
                auto m = hitRecord.material;
                auto pbm = std::dynamic_pointer_cast<Material_PBM>(m);
                
                radiance = Shade(hitRecord.hitPoint,Vector3(0, 0, 0) - ray.direction,hitRecord.normal ,hitRecord.t,pbm);
                radiance = Vector3
                (
                   std::max(0.0 , std::min(radiance.x, maxLi.x)),
                   std::max(0.0 , std::min(radiance.y, maxLi.y)),
                   std::max(0.0 , std::min(radiance.z, maxLi.z))
                );
                film.AddSample(i, j, radiance);
            }
        });
        
        Profiler profiler("渲染一轮,模式 -DebugCompileMode -MultiThread -RR:0.7 -ImportanceSampling:off -SAH:off");
        
        currentSSP += increaseStep;
        threadPool.WaitAll();
        film.Save_P3("outputP3.ppm");
        //singleDebugFilm.Save_P3("singleDebugFilm.ppm");
        //debugFilm.Save_P3();
        //film.Save_P6("outputP6.ppm");
        std::cout << "渲染进度:" << currentSSP << "/" << SAMPLE_COUNT << "\n"<< std::endl;
    }
    film.Save_P3("outputP3.ppm");
    //film.Save_P6("outputP6.ppm");
    std::cout << "渲染完成\n"<< std::endl;

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

   

    return 0;
}

