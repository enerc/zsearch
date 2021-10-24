#include "importFileBase.hpp"
#include "indexRepository.hpp"
#include "datajoin.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#include <utility>

using namespace std;
using namespace indexes;
using namespace mapping;

namespace repository {

ImportFileBase::ImportFileBase(std::string _path,std::string _index,std::shared_ptr<queries::QueryStatus> &_queryStatus) :
        path(std::move(_path)),
        index(std::move(_index)),
        queryStatus(_queryStatus){
    usefullBufferSize[0] = 0;
    usefullBufferSize[1] = 0;
    id = IndexDefinitionManager::getInstance()->getIndex(index);
}

void  ImportFileBase::processingLoop(WorkerInitParam *o) {
	IndexRepository::ingest(o->index,o->lines,queryStatus);
}

bool ImportFileBase::openFile() {
    struct stat stat_buf{};
    int rc = stat(path.c_str(), &stat_buf);
    if (rc != 0) {
    	Log::error("Import file FAILED for file "+path);
    	return false;
	}
    len = stat_buf.st_size;
#if USE_O_DIRECT
   	fd =  open(path.c_str(), O_RDONLY|O_DIRECT|O_SYNC);
#else
	fd =  open(path.c_str(), O_RDONLY);
#endif
	if (fd < 0 ) {
    	Log::error("Import file FAILED for file "+path+ " cannot open this file");
    	return false;
	}
	return true;
}

bool  ImportFileBase::initImport() {
	processor_count = thread::hardware_concurrency() - 1;
	if (processor_count == 0) {
		processor_count = 1;
	}

	workerInitParams  = nullptr;
	workerInitParams = (WorkerInitParam*) malloc(processor_count*sizeof(WorkerInitParam));
	if (workerInitParams == nullptr) {
		Log::error("Failed to create file import workers. Allocated "+ to_string(processor_count*sizeof(WorkerInitParam)));
		return false;
	}
	return true;
}

bool ImportFileBase::estimateLineWorkSize() {
	const char *p = (const char*)memchr((const void*)buffers[workingBuffer],0xa,usefullBufferSize[workingBuffer]);
	if (p == nullptr) {
	   	Log::error("Import file "+path+ " has no new line character");
		return false;
	}
	lineSize= p - buffers[workingBuffer];

	// estimated work size
	lines_per_thread = IMPORT_BUFFER_SIZE / lineSize / processor_count;
	return true;
}

bool ImportFileBase::doImport() {
	if (!initImport())
		return false;
	if (!openFile())
		return false;
	if (!decodeHeader())
		return false;
	if (!allocBuffers())
		return false;
	if (!fetch()) {
		free(buffer0);
		buffer0 = nullptr;
		free(buffer1);
		buffer1 = nullptr;
		return false;
	}
	estimateLineWorkSize();
	position = 0;

	while (len >= 0) {
		pos = toSkip;
		char *next = buffers[workingBuffer]+toSkip;
        IndexRepository::getInstance()->adjustStorageSize(index);

		for (int i=0; i< processor_count; i++) {
			char *start_pos = next;

			int m = 0;
			while (pos < usefullBufferSize[workingBuffer]) {
				char *prev = next;
				next = (char*)memchr((const void*)next,0xa,usefullBufferSize[workingBuffer]-pos);
				if (next == nullptr) {
					next = prev;
					break;
				}
				for (int k=0; k < lineSize ;  k+=64) {
					__builtin_prefetch (next+2*lineSize+k, 0, 0);
				}
				next ++;
				if (*next == 0x0d)
					next ++;
				pos += next - prev;
				m++;
				if (m == lines_per_thread-1)
					break;
			}
			if (m > 0) {
				new(&workerInitParams[i]) WorkerInitParam();
				workerInitParams[i].index = index;
				workerInitParams[i].t = this;
				workerInitParams[i].lines = string_view(start_pos,next-start_pos-1);
				pthread_attr_t attribute;
				pthread_t t;
				pthread_attr_init(&attribute);
				pthread_attr_setstacksize(&attribute,4096);
				pthread_create(&t,&attribute,ImportFileBase::InternalThreadEntryFunc,workerInitParams+i);
				threads.push_back(t);
			}
		}
		position += pos;
		fetch();
		for (auto &i : threads) {
			void* status;
			pthread_join(i, &status);
		}
		threads.clear();
	}
	free(workerInitParams);
	workerInitParams = nullptr;
	free(buffer0);
	buffer0 = nullptr;
	free(buffer1);
	buffer1 = nullptr;

	IndexRepository::getInstance()->sync();
	DataJoinFactory::free();
	return true;
}

bool ImportFileBase::allocBuffers() {
	buffer0 = (char*) aligned_alloc(getpagesize(),IMPORT_BUFFER_SIZE);
	if (buffer0 == nullptr) {
    	Log::error("Import file FAILED for file "+path+ " cannot allocate buffer memory");
    	return false;
	}
	buffers[0] = buffer0;
	buffer1 = (char*) aligned_alloc(getpagesize(),IMPORT_BUFFER_SIZE);
	if (buffer1 == nullptr) {
    	Log::error("Import file FAILED for file "+path+ " cannot allocate buffer memory");
    	free(buffer0);
    	return false;
	}
	buffers[1] = buffer1;
	workingBuffer = 0;
	return true;
}


}
