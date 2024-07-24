/*
 * @Author: Vanish
 * @Date: 2024-06-01 23:45:49
 * @LastEditTime: 2024-07-14 12:57:10
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#include "Bick.h"
#include "Ray.h"
#include "Random.h"
#include "Camera.h"



Vector3 Ray::Trace(Interval interval)
{
    Ray ray_In = *this;
    if(Debuger::DebugMode()) return Debuger::DebugResult(*this);

#ifdef DEBUG_TRACERAY
    if(depth == 0 && Random::consoleOutPutEnable) std::cout <<"\033[34m"<< "[光线开始追踪]" <<"\033[m"<<"==>";
#endif

    if(Random::GetRandomDouble(0,1) > RUSSIAN_ROULETTE_PROBABILITY && depth >0) 
    {
#ifdef DEBUG_TRACERAY
        if(Random::consoleOutPutEnable)
            std::cout <<"\033[32m"<< "光线终止追踪：俄罗斯轮盘"<<"\033[m"<<"||||||";
#endif
        return Vector3(0, 0, 0);
    }

    HitRecord hitRecord;
    g_Scene->Hit(*this, interval,hitRecord);


    if(!hitRecord.hitted)
    {
#ifdef DEBUG_TRACERAY
        if(Random::consoleOutPutEnable)
            std::cout <<"\033[32m"<< "光线终止追踪：未击中物体"<<"\033[m"<<"||||||";
#endif
        return Vector3(0, 0, 0);
    }
#ifdef DEBUG_TRACERAY
    if(Random::consoleOutPutEnable)
        std::cout << "击中物体"<< hitRecord.obj->name << " ==>";
#endif
    
     
    this->t = hitRecord.t;
    Vector3 result = Vector3(0, 0, 0);

    
    result += hitRecord.material->Shade(*this,hitRecord);
#ifdef DEBUG_TRACERAY
    if((result.x != 0 || result.y != 0 || result.z == 0) && Random::consoleOutPutEnable) std::cout << "光线第{"<<depth--<<"}次弹射结果:"<<"\033[33m"<<"(" <<result.x << "," << result.y << "," << result.z <<")"<<"\033[m"<<"<--";
#endif
    return result/RUSSIAN_ROULETTE_PROBABILITY;
}
