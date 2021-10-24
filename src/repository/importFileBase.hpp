#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

#include "../queries/queryStatus.hpp"
#include "../mapping/indexDefinitionManager.hpp"

namespace repository {

class ImportFileBase;
typedef struct {
	std::string	index;
	std::string_view lines;
	ImportFileBase	*t;
} WorkerInitParam;

class ImportFileBase {
public:
	ImportFileBase(std::string _path,std::string _index,std::shared_ptr<queries::QueryStatus> &_queryStatus);
	bool  openFile();
	bool  initImport();
	bool  estimateLineWorkSize();
	bool  doImport();
	bool  allocBuffers();

	virtual bool  fetch() = 0;
	virtual bool  decodeHeader() = 0;
	virtual ~ImportFileBase() = default;

protected:
	uint32_t processor_count = 1;
	WorkerInitParam * workerInitParams  = nullptr;
	std::vector<pthread_t> threads;
	int64_t len = 0;
	char *buffer0 = nullptr;
	char *buffer1 = nullptr;
	char *buffers[2] {};
	size_t	usefullBufferSize[2]{};
	uint32_t workingBuffer = 0;
	uint32_t lines_per_thread = 0;
	int		 fd = 0;
	size_t 	 position = 0;
	uint32_t toSkip = 0;
	uint32_t lineSize = 0;
	std::string path;
	std::string index;
	size_t pos = 0;
	std::shared_ptr<queries::QueryStatus> queryStatus;
    std::shared_ptr<mapping::IndexDefinition> id;

	static void * InternalThreadEntryFunc(void * p) {
		auto *o = (WorkerInitParam*)p;
		o->t->processingLoop(o);
		return nullptr;
	}
	void  processingLoop(WorkerInitParam *o);
};

}
