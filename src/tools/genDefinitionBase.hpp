#pragma once

#include <unistd.h>
#include <mutex>
#include <vector>
#include <list>
#include <thread>
#include "../repository/importFileBase.hpp"

namespace tools {

typedef struct {
    std::string	index;
    std::string_view lines;
    std::map<std::string,mapping::mapping_entry> maps;
    std::shared_ptr<queries::QueryStatus> queryStatus;
    GenDefinitionBase	*t;
} WorkerInitParam;

class  GenDefinitionBase {
public:
    std::string doBaseGenImport(repository::ImportFileBase *);
private:
    WorkerInitParam * workerInitParams  = nullptr;

    static void * InternalThreadEntryFunc(void * p) {
        auto *o = (WorkerInitParam*)p;
        o->t->processingLoop(o);
        return nullptr;
    }
    static void  processingLoop(WorkerInitParam *o);
    static void decodeDouble(double a, const std::string &, mapping::mapping_entry &);
    static void decodeInt(int64_t a, mapping::mapping_entry &);
    static void decodeString(const std::string_view &, mapping::mapping_entry &) ;
    [[nodiscard]] static std::string outputDefinition(const std::map<std::string,mapping::mapping_entry>&);

};


}
