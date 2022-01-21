#include "threadPool.hpp"
#include <unistd.h>
#include "../../common/log.hpp"
#include "../../queries/model/root.hpp"

using namespace queries::model;
using namespace std;

namespace devices {

ThreadPool* ThreadPoolManager::instance = nullptr;
static std::vector<sem_t> sems;
static std::vector<ThreadParam> params;
static volatile uint64_t *active;

void * ShutdownTask(void* threadData) {
    pthread_exit(nullptr);
}

void *ThreadFunc(void* threadData) {
    const uint32_t i = *(uint32_t*)threadData;

    while (true) {
        sem_wait(&sems[i]);
        switch (params[i].action) {
            case actionExecShutdown:
                (params[i].func)(&params[i]);
                break;
            case actionExecShaderOnCpu: {
                RootModel *b = (RootModel*)params[i].thisClass;
                b->addResultCount(b->execShaderThreadOnCpu(params[i].me,params[i].nbThreads));
                break;
            }
        }
        active[i] = false;
    }
    return (void*) nullptr;
}

ThreadPool::ThreadPool(uint32_t nbThreads) {
    threads.resize(nbThreads);
    params.resize(nbThreads);
    active = (uint64_t*)malloc(nbThreads*sizeof(uint64_t));
    sems.resize(nbThreads);
    threadIds.resize(nbThreads);

    for (int i=0; i< threads.size(); i++) {
        active[i] = false;
        int s = sem_init(&sems[i],PTHREAD_PROCESS_SHARED,0);
        if (s != 0) {
            Log::error("Failed to create ThreadPool semaphore");
        }
        threadIds[i] = i;
    }

    for (int i=0; i< threadIds.size(); i++) {
        pthread_t t;
        pthread_create(&t, nullptr, ThreadFunc, &threadIds.at(i));
        threads[i]= t;
        cpu_set_t set;
        CPU_ZERO( &set );
        CPU_SET( i, &set );
        pthread_setaffinity_np(t, sizeof(&set), &set);
    }
}

ThreadPool::~ThreadPool() {
    free((void *) active);
}

void ThreadPool::addTask(const ThreadParam &p) {
    while (true) {
        // Look for an available node
        for (int i=0; i< threads.size(); i++) {
            if (!active[i]) {
                active[i] = true;
                params[i] = p;
                sem_post(&sems[i]);
                return;
            }
        }
        // all threads busy
        usleep(1000L);
    }
}

void ThreadPool::wait() {
    bool working = true;
    while (working) {
        working = false;
        for (int i=0; i< threads.size(); i++) {
            working |= active[i];
        }
        if (working) {
            usleep(1000L);
        }
    }
}

void ThreadPool::shutdown() {
    ThreadParam p;
    p.func = ShutdownTask;
    p.action = actionExecShutdown;
    for (int i=0; i< threadIds.size(); i++) {
        params[i] = p;
        sem_post(&sems[i]);
    }
    for (int i=0; i< threadIds.size(); i++) {
        void* status;
        pthread_join(threads[i],&status);
    }
    threads.clear();
    sems.clear();
    params.clear();
}

}
