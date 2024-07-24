/*
 * @Author: Vanish
 * @Date: 2024-06-14 20:44:44
 * @LastEditTime: 2024-07-09 16:45:31
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#pragma once

#include <random>
#include <cstdlib>
#include <ctime>


class Random 
{
public:
    static int GetRandomInt(int min, int max);
    static double GetRandomDouble(double min, double max);
    static bool consoleOutPutEnable;
};