#pragma once

#include <vector>
#include <pthread.h>
#include <cstdint>
#include <thread>
#include <mutex>
#include <semaphore.h>

namespace devices {

enum ThreadAction {
    actionExecShutdown,
    actionExecShaderOnCpu
};

typedef void* (*ThreadEntryFunc)(void * p) ;

struct ThreadParam {
    ThreadAction action;
    ThreadEntryFunc func;
    void *thisClass;
    uint32_t me;
    uint32_t nbThreads;
};

class ThreadPoolManager;

class ThreadPool {
public:
    explicit ThreadPool(uint32_t nbThreads);
    ~ThreadPool();
    void     addTask(const ThreadParam &);
    void     wait();
    uint32_t getPoolSize() const  { return threads.size(); }
private:
    std::vector<pthread_t> threads;
    std::vector<uint32_t>threadIds;
    void shutdown();
    friend ThreadPoolManager;
};

class ThreadPoolManager {
public:
    static ThreadPool* getInstance() {
        if (instance == nullptr)  {
            instance = new ThreadPool(std::max(std::thread::hardware_concurrency(),(unsigned int)1));
        }
        return instance;
    }
    static void  shutdown() {
        if (instance != nullptr) {
            instance->shutdown();
        }
    }
private:
    static ThreadPool *instance;
};

}