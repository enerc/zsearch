#include <cstring>
#include <cmath>
#include "../../src/common/tools.hpp"
#include "../../src/repository/storageModel.hpp"
#include "../../src/devices/cpu/codegen.hpp"
#include "../../src/repository/indexRepository.hpp"

using namespace std;
using namespace indexes;
using namespace devices;

static const char *color1 = "\e[38;5;85m";
static const char *color2 = "\e[38;5;196m";
static const char *color3 = "\e[38;5;154m";
static const char *color4 = "\e[38;5;245m";
static void *storage;
static void *storageIsDeleted;
static __uint128_t mask;
static __uint128_t T1, T2;
static double frequency;
static long seedInit;
static uint16_t nullTest, dummy;
static bool isSigned = true;
static bool doBench = true;
constexpr long BENCH_LOOPS = 1000L*1000L*1L;
constexpr long CHECK_LOOPS = 1000L*1L;
static __m256i banks[MAX_BANKS*8];
static void *sources[2];

#define DEBUG false

static uint32_t mrand() {return rand() ^ rand() << 30; }
static void msrand(uint64_t i) {srand (i);}

static void cpuClock() {
    uint64_t t0 = now();
    __m256i a = _mm256_set_epi32(1,2,3,4,5,6,7,8);
    for (long i=0; i < 1000L*1000L*1000L*10L; i++) {
        a = _mm256_add_epi32(a,a);
    }
    banks[0] = _mm256_add_epi32(a,banks[0]);
    uint64_t tf = now();
    frequency = 1/((double)(tf-t0)/1000.0/1000.0/10.0);
    cout << color1 << "Frequency : " << frequency << " Ghz" << endl;
}

typedef struct {
    Codegen *codegen;
    __m256i input[32];
    __m256i banks[32];
} TaskData;

static void * benchKernelFunc(void * p) {
    auto taskData = (TaskData*)p;
    uint32_t r1 = 0;
    void *src[2];
    src[0] = taskData->input;
    for (long h = 0; h < BENCH_LOOPS; h++) {
        r1 ^= taskData->codegen->exec(src,taskData->banks);
    }
    dummy ^= r1;
    return nullptr;
}

static __uint128_t sign(const __uint128_t &a,uint32_t len) {
    if (!isSigned) {
        if (len < 128) {
            return a;
        } else {
            return (a<<1)>>1;
        }
    }
    if (len < 128) {
        return SignExtend(a,len);
    } else {
        auto b = (__int128_t)a;
        if (b > 0) {
            return (a<<2)>>2;
        } else {
            b = -b;
            b = (b<<2)>>2;
            return -b;
        }
    }
}

static __uint128_t nextVal() {
    __uint128_t r = 0;
    r = mrand();
    r |= (__uint128_t)mrand() << 32;
    r |= (__uint128_t)mrand() << 64;
    r |= (__uint128_t)mrand() << 96;
    r &= mask;
    return r;
}
#if 0
inline __m256i unsigned_lessthan(__m256i a, __m256i b) {
    __m256i sign64 = _mm256_set1_epi64x(0x8000000000000000L);
    __m256i aflip = _mm256_xor_si256(a, sign64);
    __m256i bflip = _mm256_xor_si256(b, sign64);
    __m256i cmp = _mm256_cmpgt_epi64(aflip,bflip);
    return cmp;
}

__m256i sub(__m256i x, __m256i y) {
    __m256i z = _mm256_sub_epi64(x,y);
    __m256i c = _mm256_unpacklo_epi64(_mm256_setzero_si256(), unsigned_lessthan(z,x));
    z = _mm256_add_epi64(z,c);
    return z;
}
#endif
static void setMask(int i) {
    mask = (((__uint128_t)1) << i) -1;
    if (i==128) {
        mask = 0;
        mask --;
    }
}

static bool checkStorage() {
    for (int i = 1; i <= 128; i++) {
        memset(storage,0,1024);
        std::shared_ptr<StorageModel> storageModel = StorageModelFactory::getModel(storage,storageIsDeleted,i);
        setMask(i);

        // fill
        srand(seedInit);
        for (int j= 0; j < 16; j++) {
            __uint128_t v = nextVal();
            storageModel->save(j,v,(nullTest>>j)&1, true);
        }

        // check
        srand(seedInit);
        for (int j= 0; j < 16; j++) {
            __uint128_t v = nextVal();
            __uint128_t r = 0;
            bool isNull;
            bool endOfDocument;
            bool isDeleted;
            storageModel->get(j,r,isNull, endOfDocument,isDeleted);
            if (isNull != ((nullTest>>j)&1) || endOfDocument != true || r != v) {
                cout << dec << color2 << "Check FAILED for " << i << ":" << j << " expected " << hex << (uint64_t)(v>>64) << (uint64_t)v << " found " << (uint64_t)(r>>64) << (uint64_t)r <<  endl;
                return false;
            }
        }
    }
    return true;
}

template<TestOper oper>bool checkKernel(int iter) {
    StorageModelFactory storageModelFactory;
    int end = 128;
    int start = 1;
    bool isFloat = false;

    mapping::storage_types dt1 = isSigned ? mapping::storage_type_signed : mapping::storage_type_unsigned;
    if (oper == OperLTF16 || oper == OperGTF16 || oper == OperLEF16 || oper == OperGEF16 || oper == OperEqualF16 || oper == OperNotEqualF16) {
        start = 16;
        end = 16;
        isFloat = true;
    }
    if (oper == OperLTF32 || oper == OperGTF32 || oper == OperLEF32 || oper == OperGEF32 || oper == OperEqualF32 || oper == OperNotEqualF32) {
        start = 32;
        end = 32;
        isFloat = true;
    }
    if (oper == OperLTF64 || oper == OperGTF64 || oper == OperLEF64 || oper == OperGEF64 || oper == OperEqualF64 || oper == OperNotEqualF64) {
        start = 64;
        end = 64;
        isFloat = true;
    }

    for (int len = start; len <= end; len++) {
        memset(storage,0,1024);
        std::shared_ptr<StorageModel> storageModel = StorageModelFactory::getModel(storage,storageIsDeleted,len);
        setMask(len);
        if (DEBUG) cout << endl << endl;

        srand(seedInit);
        for (int j= 0; j < 16; j++) {
            __uint128_t v = nextVal();
            cout << color1;
            if (j == 5) {
                if (DEBUG) cout << color2 ;
                T1 = sign(v,len) ;
            }
            if (j == 14) {
                if (DEBUG) cout << color2 ;
                T2 = sign(v,len) ;
            }
        }
        if (DEBUG) cout << " isNull:" << nullTest << " i=" << dec << len << " iter#" << iter << endl;
        // fill
        srand(seedInit);
        uint16_t theResult1 = 0;
        uint16_t theResult2 = 0;
        for (int j= 0; j < 16; j++) {
            __uint128_t v = sign(nextVal(),len);
            bool b = ((nullTest>>j)&1) == 0;
            if (oper(v, T1) && b) {
                theResult1 |= 1U << j;
            }
            if (oper(v, T2) && b) {
                theResult2 |= 1U << j;
            }
            if (DEBUG) cout << dec << color1 << "Save "  << ":" << j << " = " << hex << (uint64_t)((v&mask)>>64) << (uint64_t)(v&mask)<<  endl;
            storageModel->save(j,v&mask,(nullTest>>j)&1, true);
        }
        // checks
        OperatorType o = storageModelFactory.convertToOperatorType<oper>();
        uint32_t r1;
        {
            Codegen codegen;
            codegen.fetchVariable(len,isFloat,isSigned,len,isFloat,0,0,1);
            codegen.loadConstant(T1,len,isFloat,1,false);
            codegen.cmp(o,len,isFloat,isSigned,0,1);
            codegen.flush();
            sources[0] = storage;
            r1 = codegen.exec(sources,banks);
        }
        if (r1 != theResult1) {
            cout << color2 << endl << dec << len << "/" << end << " Found:" << hex << r1 << " expecting(1):" << theResult1 << " isNull:" << nullTest << endl;
            exit(0);
        }

        uint32_t r2;
        {
            Codegen codegen;
            codegen.fetchVariable(len,isFloat,isSigned,len,isFloat,0,0,1);
            codegen.loadConstant(T2,len,isFloat,1,false);
            codegen.cmp(o,len,isFloat,isSigned,0,1);
            codegen.flush();
            sources[0] = storage;
            r2 = codegen.exec(sources,banks);
        }

        if (r2 != theResult2) {
            cout << color2 << endl << dec << len << "/" << end << " Found:" << hex << r2 << " expecting(2):" << theResult2 << " isNull:" << nullTest << endl;
            exit(0);
        }
    }
    _mm256_zeroupper();
    return true;
}

void benchKernelIN32() {
    Codegen codegen;
    std::vector<uint32_t> v;
    for (int i = 0; i < 8; i++) {
        __uint128_t k = nextVal();
        v.push_back(k);
        codegen.loadConstant(k,32,false,0,true);
    }
    codegen.fetchVariable(32, false, isSigned, 32,false,0, 0,1);
    codegen.in32(0);
    codegen.flush();
    sources[0] = storage;

    // perf
    uint64_t t0 = now();
    uint64_t thrs = thread::hardware_concurrency();
    vector<pthread_t> threads;
    vector<TaskData> tasks;
    for (int i=0; i< thrs; i++) {
        TaskData td;
        td.codegen = &codegen;
        tasks.push_back(td);
    }
    for (int i=0; i< thrs; i++) {
        pthread_t t;
        pthread_create(&t, nullptr, benchKernelFunc, &tasks.at(i));
        threads.push_back(t);
    }
    for (auto &i : threads) {
        void *status;
        pthread_join(i, &status);
    }
    uint64_t tf = now();
    tasks.clear();
    threads.clear();
    double kps = thrs * 16 * (double) BENCH_LOOPS / (double) (tf - t0) / 1000.0;
    double cycles = frequency * (double) (tf - t0) * 1000 / (double) BENCH_LOOPS / 16.0;
    cout << fixed << setprecision(2);
    cout << color4 << dec << " Kernel#" << 32 << " " << kps << "10^9  rows/sec ("<<thrs<<" thread) " << cycles << " cycles/row - 8 choices in the IN statement" << endl;
}

static bool doCheckKernelIN32(bool isSigned) {
    Codegen codegen;
    vector<__uint32_t>v;

    memset(storage, 0, 1024);
    std::shared_ptr<StorageModel> storageModel = StorageModelFactory::getModel(storage, storageIsDeleted, 32);
    setMask(32);
    srand(seedInit);

    for (int i = 0; i < 16; i++) {
        __uint128_t  k = nextVal();
        v.push_back(k);
    }

    int k = mrand() & 15;
    __uint128_t a = v.at(k);
    codegen.loadConstant(a,32,false,1,true);
    int theResult1 = 1 << k;

    for (int j = 0; j < 16; j++) {
        uint32_t b = v.at(j);
        storageModel->save(j, b, (nullTest >> j) & 1, true);
    }
    codegen.fetchVariable(32, false, isSigned, 32, false,0, 0,1);
    codegen.in32(0);
    codegen.flush();
    sources[0] = storage;

    uint32_t r1 = codegen.exec(sources,banks);

    if (r1 != theResult1) {
        cout << color2 << endl << dec << 32 << "/" << 128 << " Found:" << hex << r1 << " expecting(1):" << theResult1 << " isNull:" << nullTest << endl;
        exit(0);
    }
    return true;
}

static void checkKernelIN32(const string&testName, bool isSigned) {
    seedInit = 0x5cf9a467;
    cout << "Check kernels " << testName << " ... " << flush;
    for (long i=0; i< CHECK_LOOPS; i++) {
        doCheckKernelIN32(isSigned);
    }
    cout << "passed" << endl;
    if (doBench) benchKernelIN32();
}


void benchKernelIN64() {
    Codegen codegen;
    vector<__uint32_t>v;

   for (int i = 0; i < 8; i++) {
        __uint128_t  k = nextVal();
        v.push_back(k);
    }
    codegen.fetchVariable(64, false, isSigned, 64, false, 0, 0,1);
    codegen.in64(0);
    codegen.flush();
    sources[0] = storage;

    // perf
    uint64_t t0 = now();
    uint64_t thrs = thread::hardware_concurrency();
    vector<pthread_t> threads;
    vector<TaskData> tasks;
    for (int i=0; i< thrs; i++) {
        TaskData td;
        td.codegen = &codegen;
        tasks.push_back(td);
    }
    for (int i=0; i< thrs; i++) {
        pthread_t t;
        pthread_create(&t, nullptr, benchKernelFunc, &tasks.at(i));
        threads.push_back(t);
    }
    for (auto &i : threads) {
        void *status;
        pthread_join(i, &status);
    }
    uint64_t tf = now();
    tasks.clear();
    threads.clear();
    double kps = thrs * 16 * (double) BENCH_LOOPS / (double) (tf - t0) / 1000.0;
    double cycles = frequency * (double) (tf - t0) * 1000 / (double) BENCH_LOOPS / 16.0;
    cout << fixed << setprecision(2);
    cout << color4 << dec << " Kernel#" << 32 << " " << kps << "10^9  rows/sec ("<<thrs<<" thread) " << cycles << " cycles/row - 8 choices in the IN statement" << endl;
}

static bool doCheckKernelIN64(bool isSigned) {
    std::vector<uint64_t> v;
    Codegen codegen;

    memset(storage, 0, 1024);
    std::shared_ptr<StorageModel> storageModel = StorageModelFactory::getModel(storage, storageIsDeleted, 64);
    setMask(64);
    srand(seedInit);

    for (int i = 0; i < 16; i++) {
        v.push_back(nextVal());
    }

    int k = mrand() & 15;
    __uint128_t a = v.at(k);
    codegen.loadConstant(a,64,false,0,true);
    int theResult1 =  1 <<  k;

    for (int j = 0; j < 16; j++) {
        uint64_t b = j == k ? v.at(k) : nextVal();
        storageModel->save(j, b, (nullTest >> j) & 1, true);
    }
    codegen.fetchVariable(64, false, isSigned,64, false, 0, 0,1);
    codegen.in64(0);
    codegen.flush();
    sources[0] = storage;

    uint32_t r1 = codegen.exec(sources,banks);

    if (r1 != theResult1) {
        cout << color2 << endl << dec << 64 << "/" << 128 << " Found:" << hex << r1 << " expecting(1):" << theResult1 << " isNull:" << nullTest << endl;
        exit(0);
    }
    return true;
}

static void checkKernelIN64(const string&testName, bool isSigned) {
    seedInit = 0x5cf9a467;
    cout << "Check kernels " << testName << " ... " << flush;
    for (long i=0; i< CHECK_LOOPS; i++) {
        doCheckKernelIN64(isSigned);
    }
    cout << "passed" << endl;
    if (doBench) benchKernelIN64();
}


static float distanceToCenter;
static float CenterX,CenterY;
static float st_distance_sphere(const int32_t pt1[2]) {
    float A = sinf(((CenterY - pt1[1]*(1.0/(double)GEOMETRY_SCALING_LAT)) * M_PI / 180.) / 2.);
    float B = sinf(((CenterX - pt1[0]*(1.0/(double)GEOMETRY_SCALING_LON)) * M_PI / 180.) / 2);
    float d = 6371000.0f * 2 * asin(sqrt(A*A + cosf(pt1[1]*(1.0/(double)GEOMETRY_SCALING_LAT) * M_PI / 180.) * cosf(CenterY* M_PI / 180.0) * B*B));
    return d;
}

bool checkKernelDistanceSphere() {
    int len = 64;
    memset(storage,0,1024);
    std::shared_ptr<StorageModel> storageModel = StorageModelFactory::getModel(storage,storageIsDeleted,len);
    setMask(len);
    __uint128_t distanceSphere256;
    *(float*)&distanceSphere256 = distanceToCenter;
    int32_t t;
    uint32_t m = 0;
    const float SC = 400000000.0;
    Codegen codegen;

    __uint128_t v = nextVal();
    CenterX  = -70+((int32_t)sign(v,32))/SC;
    CenterY = 45+((int32_t)sign(v,32))/SC;
    __uint128_t ce = locationToUint64(CenterX, CenterY);
    codegen.loadConstant(ce,64,false,0,true);
    codegen.loadConstant(distanceSphere256,32,true,0,false);

    srand(seedInit);
    for (int j= 0; j < 16; j++) {
        int32_t z[2];
        v = nextVal();
        double d = -71+(((int32_t)sign(v,32))/SC);
        z[0] = d * GEOMETRY_SCALING_LON;
        d = 44+((int32_t)sign(v,32))/SC;
        z[1] = d * GEOMETRY_SCALING_LAT;
        bool b = st_distance_sphere(z) < distanceToCenter;
        if (b) {
            m |= 1 << j;
        }
        //cout << hex << "Save : " << *(uint64_t *) z << " distance :" << dec << st_distance_sphere(t[j]) <<  " " << (st_distance_sphere(t[j]) < distanceToCenter ) << endl;
        storageModel->save(j, *(uint64_t *) z, (nullTest >> j) & 1, true);
    }
    codegen.fetchVariable(64,false,false,64,false,0,1,1);
    codegen.distanceSphere(1,32);
    codegen.cmp(OperLTType,32,true,false,1,0);
    //codegen.illegalInstruction();
    codegen.flush();
    sources[0] = storage;

    uint32_t r = codegen.exec(sources,banks);
    if (r != m) {
        cout << color2 << "Expecting " << hex << m << "/" << r << dec << " distance=" << distanceToCenter <<  endl;
        Codegen::dumpFloat(0,banks);
        Codegen::dumpFloat(1,banks);
        exit(0);
    }
    _mm256_zeroupper();
    return true;
}

void benchDistanceSphere() {
    Codegen codegen;
    __uint128_t ce = locationToUint64(CenterX, CenterY);
    codegen.loadConstant(ce,64,false,0,true);
    __uint128_t distanceSphere256;
    *(float*)&distanceSphere256 = distanceToCenter;
    codegen.loadConstant(distanceSphere256,32,true,1,false);
    codegen.fetchVariable(64,false,false,64,false,0,2,1);
    codegen.distanceSphere(2,32);
    codegen.cmp(OperLTType,32,true,false,2,1);
    codegen.flush();
    sources[0] = storage;

    // perf
    uint64_t t0 = now();
    uint64_t thrs = thread::hardware_concurrency();
    vector<pthread_t> threads;
    vector<TaskData> tasks;
    for (int i=0; i< thrs; i++) {
        TaskData td;
        td.codegen = &codegen;
        tasks.push_back(td);
    }
    for (int i=0; i< thrs; i++) {
        pthread_t t;
        pthread_create(&t, nullptr, benchKernelFunc, &tasks.at(i));
        threads.push_back(t);
    }
    for (auto &i : threads) {
        void *status;
        pthread_join(i, &status);
    }
    uint64_t tf = now();
    tasks.clear();
    threads.clear();
    double kps = thrs * 8 * (double) BENCH_LOOPS / (double) (tf - t0) / 1000.0;
    double cycles = frequency * (double) (tf - t0) * 1000 / (double) BENCH_LOOPS / 8.0;
    cout << fixed << setprecision(2);
    cout << color4 << dec << "Kernel on sphere distance#" " " << kps << " 10^9 rows/sec ("<<thrs<<" threads) " << cycles << " CPU cycles/row  " << endl;
}

void doCheckDistanceSphere() {
    seedInit = 0x5cf9a467;
    cout << "Check kernels Distance Sphere  ... " << flush;
    for (long i=0; i< CHECK_LOOPS; i++) {
        srand(seedInit);
        seedInit = mrand();
        nullTest = 0;
        distanceToCenter = 3000;// rand() % 300000;
        checkKernelDistanceSphere();
    }
    cout << "passed" << endl;
    if (doBench) benchDistanceSphere();
}


template<TestOper oper>void benchKernel() {
    mapping::storage_types dt1 = isSigned ? mapping::storage_type_signed : mapping::storage_type_unsigned;
    StorageModelFactory storageModelFactory;
    int e = 128;
    int start = 1;
    bool isFloat = false;
    if (oper == OperLTF16 || oper == OperGTF16 || oper == OperLEF16 || oper == OperGEF16 || oper == OperEqualF16 || oper == OperNotEqualF16) {
        start = 16;
        e = 16;
        isFloat = true;
    }
    if (oper == OperLTF32 || oper == OperGTF32 || oper == OperLEF32 || oper == OperGEF32 || oper == OperEqualF32 || oper == OperNotEqualF32) {
        start = 32;
        e = 32;
        isFloat = true;
    }
    if (oper == OperLTF64 || oper == OperGTF64 || oper == OperLEF64 || oper == OperGEF64 || oper == OperEqualF64 || oper == OperNotEqualF64) {
        start = 64;
        e = 64;
        isFloat = true;
    }
    for (int len = start; len <= e; len++) {
        OperatorType o = storageModelFactory.convertToOperatorType<oper>();
        Codegen codegen;
        codegen.cmp(o, len, isFloat, isSigned, 0, 1);
        codegen.loadConstant(T1, len, isFloat, 1, false);
        codegen.flush();
        sources[0] = storage;
        uint64_t thrs = thread::hardware_concurrency();

        // perf
        uint64_t t0 = now();
        vector<pthread_t> threads;
        vector<TaskData> tasks;
        for (int i=0; i< thrs; i++) {
            TaskData td;
            td.codegen = &codegen;
            tasks.push_back(td);
        }
        for (int i=0; i< thrs; i++) {
            pthread_t t;
            pthread_create(&t, nullptr, benchKernelFunc, &tasks.at(i));
            threads.push_back(t);
        }
        for (auto &i : threads) {
            void *status;
            pthread_join(i, &status);
        }
        uint64_t tf = now();
        tasks.clear();
        threads.clear();
        double kps = thrs * 16 * (double) BENCH_LOOPS / (double) (tf - t0) / 1000.0;
        double cycles = frequency * (double) (tf - t0) * 1000 / (double) BENCH_LOOPS / 16.0;
        cout << fixed << setprecision(2);
        cout << color4 << dec << "  bench kernel#" << len << " " << kps << " 10^9 rows/sec ("<<thrs<<" threads) " << cycles << " CPU cycles/row  " << endl;
    }
    _mm256_zeroupper();
}


static int PSF(uint32_t nbEntries, const unsigned char* stringPtr, const unsigned int *offsets, uint64_t sear,uint64_t mask, const string &needle) {
    int ret = 0;
    const __m256i avx2_search_needle_first_byte = _mm256_set1_epi8(needle.front());
    const __m256i avx2_search_needle_last_byte = _mm256_set1_epi8(needle.back());
    for (int j = 0; j < nbEntries; j++) {
        auto size = offsets[1] - offsets[0];
        if (size >= needle.size()) {
            ret += avx2_strstr<5>(avx2_search_needle_first_byte, avx2_search_needle_last_byte,(const char *) (stringPtr), size, needle.c_str());
        }
        stringPtr += size;
        offsets ++;
    }
    return ret;
}

void benchKernelSearchFirst() {
    if (!doBench) return;
    auto p = IndexRepository::getInstance()->getIndex("english_words", "name");
    auto c = p->getChunkForRead(0);
    shared_ptr<dictionary::Dictionary> dic = c->getDictionary();
    if (dic == nullptr) {
        // skip if test files not there
        return;
    }
    auto stringPtr = dic->getValuePtr();
    auto sz = dic->getSizes();

    const string needle = "level";
    uint64_t sear = needle.at(0);
    uint64_t mask = 0xff;
    int q = 1;
    while (q < needle.size() && q < 8) {
        sear |= (uint64_t)needle.at(q) << (8*q);
        mask |= 0xffL << (8*q);
        q++;
    }
    int r = 0;
    auto n = now();
    for (int i=0; i < 1000; i++) {
        r += PSF(dic->getNbEntries(), stringPtr, sz,sear,mask,needle);
    }
    cout << "Dictionary lookup took "<< (now()-n) << " µS" << endl;
}

template <TestOper oper>void doCheck(const string&testName, bool isIsSigned) {
    seedInit = 0x5cf9a467;
    isSigned = isIsSigned;
    cout << "Check kernels " << testName << " ... " << flush;
    for (long i=0; i< CHECK_LOOPS; i++) {
        if (DEBUG) cout << hex << "seed = " << seedInit << endl;
        srand(seedInit);
        seedInit = mrand();
        nullTest = 0;
        checkKernel<oper>(i);
    }
    cout << color3 << "passed" << endl;
    if (doBench) benchKernel<oper>();
}
void checkKernels(bool _doBench) {
    doBench = _doBench;
    if (doBench) cpuClock();
    cout << color3 << "Testing kernels ..... " << endl;
    storage = malloc(1024);
    memset(storage,0,1024);
    storageIsDeleted  = malloc(CHUNK_SIZE/8);
    cout << "Check storage ... ";
    if (checkStorage()) {
        cout << color3 << "passed" << endl;
    } else {
        cout << color2 << "Failed" << endl;
        return;
    }
    doCheck<OperEqual>("Equal unsigned", false);
    doCheck<OperEqual>("Equal signed", true);
    doCheck<OperNotEqual>("Not Equal unsigned", false);
    doCheck<OperNotEqual>("Not Equal signed", true);
    doCheck<OperLTU>("Less than unsigned",false);
    doCheck<OperLTS>("Less than signed",true);
    doCheck<OperGTU>("Greater unsigned",false);
    doCheck<OperGTS>("Greater signed",true);
    doCheck<OperLEU>("Less or Equal unsigned",false);
    doCheck<OperLES>("Less or Equal signed",true);
    doCheck<OperGEU>("Greater or Equal unsigned",false);
    doCheck<OperGES>("Greater or Equal signed",true);
    doCheck<OperEqualF16>("Equal FP16",false);
    doCheck<OperNotEqualF16>("Not Equal FP16",false);
    doCheck<OperLEF16>("Less or equal than FP16",false);
    doCheck<OperLTF16>("Less than FP16",false);
    doCheck<OperGTF16>("Greater than FP16",false);
    doCheck<OperGEF16>("Greater or equal than FP16",false);
    doCheck<OperEqualF32>("Equal float",false);
    doCheck<OperNotEqualF32>("Not Equal float",false);
    doCheck<OperLTF32>("Less than FP32",false);
    doCheck<OperGTF32>("Greater than FP32",false);
    doCheck<OperLEF32>("Less or equal than FP32",false);
    doCheck<OperGEF32>("Greater or equal than FP32",false);
    doCheck<OperEqualF64>("Equal double",false);
    doCheck<OperNotEqualF64>("Not Equal double",false);
    doCheck<OperLTF64>("Less than FP64",false);
    doCheck<OperGTF64>("Greater than FP64",false);
    doCheck<OperLEF64>("Less or equal than FP64",false);
    doCheck<OperGEF64>("Greater or equal than FP64",false);
    doCheckDistanceSphere();
    checkKernelIN32("In array (32 bits)",false);
    checkKernelIN64("In array (64 bits)",false);
    benchKernelSearchFirst();
    free(storage);
    free(storageIsDeleted);
}

