/*
 * @Author: Vanish
 * @Date: 2024-07-09 16:43:00
 * @LastEditTime: 2024-09-08 05:02:42
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#include "Random.h"

// 初始化静态成员变量
std::mt19937 Random::generator(std::random_device{}());

int Random::GetRandomInt(int min, int max) {
    std::uniform_int_distribution<int> dis(min, max);
    return dis(generator);
}

double Random::GetRandomDouble(double min, double max) {
    std::uniform_real_distribution<double> dis(min, max);
    return dis(generator);
}

double Random::GetRandomNormalDouble(double mean, double stddev) {
    std::normal_distribution<double> dis(mean, stddev);
    return dis(generator);
}

bool Random::consoleOutPutEnable = false;