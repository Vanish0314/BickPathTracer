#pragma once

#include <vector>
#include <thread>
#include <list>
#include <functional>
#include "SpinLock.h"

class Task{
public:
    virtual void run() = 0;
};

class ThreadPool{
public:
    ThreadPool(int threadCount = 0);
    ~ThreadPool();

public:
    static void WorkerThread(ThreadPool* master);

private:
    std::vector<std::thread> m_Threads;
    std::list<Task*> m_Tasks;//list添加与删除o(1)

    SpinLock spinLock;
    std::atomic_bool alive = true; // 线程池是否存活
    std::atomic_int32_t unfinishedTasks = 0; // 未完成的任务数

public:

    void ParallelFor(int width, int height, const std::function<void(int,int)> &func);
    void WaitAll() const;

    void AddTask(Task* task);
    Task* GetTask();

};