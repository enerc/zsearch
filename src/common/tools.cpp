#include "tools.hpp"

using namespace std;

constexpr uint64_t BlocSize = 2L*1024L*1024L;

static void * parZeroed4_thr(void * p) {
    fastZeroed4(p,BlocSize);
    return nullptr;
}

void parZeroed4(void *pvDest, size_t nBytes) {
    vector<pthread_t> threads;
    while (nBytes > BlocSize) {
        pthread_t t;
        pthread_create(&t, nullptr, parZeroed4_thr, pvDest);
        threads.push_back(t);
        nBytes -=  BlocSize;
        pvDest = (void*) ((uint8_t*)pvDest + BlocSize);
    }
    if (nBytes > 0) {
        fastZeroed4(pvDest,nBytes);
    }
    for (auto &i : threads) {
        void* status;
        pthread_join(i, &status);
    }
    threads.clear();
}
