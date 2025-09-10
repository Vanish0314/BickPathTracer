#pragma once

#include <string>
#include <chrono>

class Profiler
{
public:
    Profiler(const std::string &name) : name(name), start(std::chrono::high_resolution_clock::now()){}
    ~Profiler();
private:
    std::string name;
    std::chrono::high_resolution_clock::time_point start;
};