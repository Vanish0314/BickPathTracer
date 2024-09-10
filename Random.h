/*
 * @Author: Vanish
 * @Date: 2024-06-14 20:44:44
 * @LastEditTime: 2024-09-08 05:02:35
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
    static double GetRandomNormalDouble(double mean, double stddev);
    static bool consoleOutPutEnable;

private:
    static std::mt19937 generator;
};

