/*
 * @Author: Vanish
 * @Date: 2024-09-03 18:26:27
 * @LastEditTime: 2024-09-05 22:10:49
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#include "ThreadPool.h"
#include "Bick.h"

void ThreadPool::WorkerThread(ThreadPool* master)
{
    while(master->alive){
        Task* task = master->GetTask();
        if(task != nullptr){
            task->run();
            master->unfinishedTasks--;
        }
        else{
            std::this_thread::yield(); // 让出线程给操作系统
        }
    }
}
ThreadPool::ThreadPool(int threadCount)
{
    alive = true;

    if(threadCount <= 0){
        threadCount = std::thread::hardware_concurrency() * 2; // 若线程数为0，则使用CPU核心数*2
#ifdef DEBUG_MODE
        threadCount = 1;
#endif
    }

    for(int i = 0; i < threadCount; i++){
        m_Threads.push_back(std::thread(ThreadPool::WorkerThread, this));
    }
}

ThreadPool::~ThreadPool()
{
    WaitAll();
    alive = false;

    for(auto& thread : m_Threads){
        if(thread.joinable()){
            thread.join();
        }
    }
    m_Threads.clear();
}

struct ParallelForTask : public Task
{
public:
    ParallelForTask(int x, int y, const std::function<void(int,int)> &func)
        : x(x), y(y), func(func)
    {}

    void run() override {
        func(x,y);
    }

private:
    int x,y;
    std::function<void(int,int)> func;

};


void ThreadPool::ParallelFor(int width, int height, const std::function<void(int,int)> &func)
{
    Guard guard(spinLock);
    
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            unfinishedTasks++;
            m_Tasks.push_back(new ParallelForTask(x,y,func));
        }
    }
}

void ThreadPool::AddTask(Task *task)
{
    Guard guard(spinLock);

    unfinishedTasks++;
    m_Tasks.push_back(task);
}
Task* ThreadPool::GetTask()
{
    Guard guard(spinLock);
    if(m_Tasks.empty()){
        return nullptr;
    }

    Task* task = m_Tasks.front();
    m_Tasks.pop_front();
    return task;
}

void ThreadPool::WaitAll() const {
    while(unfinishedTasks > 0){// 等待所有任务完成
        std::this_thread::yield();// 让出线程给操作系统
    } 
}