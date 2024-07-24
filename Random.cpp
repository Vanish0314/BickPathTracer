/*
 * @Author: Vanish
 * @Date: 2024-07-09 16:43:00
 * @LastEditTime: 2024-07-13 05:12:12
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#include "Random.h"

int Random::GetRandomInt(int min ,int max)
{
    return rand() % (max - min + 1) + min;
}

double Random::GetRandomDouble(double min, double max)
{
    // if((double)rand() / RAND_MAX < 0.0001)
    //     srand((int)time(NULL));  // 产生随机种子
    return (double)rand() / RAND_MAX * (max - min) + min;
}

bool Random::consoleOutPutEnable = false;