#pragma once

#include "importFileBase.hpp"
#include <unistd.h>
#include <mutex>
#include <vector>
#include <list>
#include <thread>

namespace repository {

class ImportLZ4File;
typedef struct {
	ImportLZ4File	*t;
} WorkerLZ4;

class ImportLZ4File : public ImportFileBase {
public:
	using ImportFileBase::ImportFileBase;
	~ImportLZ4File() override {
		void* status;
		pthread_join(thr,&status);
	}
	bool fetch() override;
private:
	uint32_t magic = 0;
	char 	flg = 0;
	char 	bd = 0;
	char 	hc = 0;
	bool 	blockIndependent = false;
	bool 	blocChecksumFlag = false;
	bool 	contentSizeFlag = false;
	bool 	contentChecksumFlag = false;
	bool 	dictionaryIdFlag = false;
	uint32_t blockSize = 0;
	uint64_t contentSize = 0;
	uint32_t dicId = 0;
	uint32_t endFrameSize = 0;
	std::list<std::vector<char>> cmpFrames;
	std::list<uint32_t> cmpFramesSize;
	pthread_t thr;
	bool 	 fileLoaded = false;
	size_t 	 toRead = 0;
    std::mutex listMu;
    WorkerLZ4 wo;


	bool	decodeHeader() override;

    [[maybe_unused]] void	dumpHeader();
	inline uint64_t get64() {
		uint64_t ret;
		size_t dummy = read(fd,&ret,8);
		len -= 8;
		return ret;
	}
	inline uint32_t get32() {
		uint32_t ret;
		size_t  dummy = read(fd,&ret,4);
		len -= 4;
		return ret;
	}
	inline char get8() {
		char ret;
		size_t  dummy = read(fd,&ret,1);
		len --;
		return ret;
	}
	static void * InternalThreadEntryFunc(void * p) {
		auto *o = (WorkerLZ4*)p;
		o->t->processingLZ4Loop(o);
		return nullptr;
	}
	void  processingLZ4Loop(WorkerLZ4 *);

};
}
