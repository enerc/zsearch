#include "importLZ4File.hpp"
#include "../../config.h"

#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <lz4.h>
#include <sys/mman.h>

using namespace std;

namespace repository {

// from LZ4 specs
constexpr uint32_t BlocSize[] = { 0,0,0,0, 1<<16, 1<<18, 1<<20, 1<<22 };


bool ImportLZ4File::decodeHeader() {
	close(fd);
	fd =  open(path.c_str(), O_RDONLY);
	magic = get32();
	flg = get8();
	blockIndependent = (flg>>5) & 1;
	blocChecksumFlag = (flg>>4) & 1;
	contentSizeFlag = (flg>>4)&1;
	contentChecksumFlag = (flg>>2)&1;
	dictionaryIdFlag = flg&1;
	endFrameSize = 4+ (contentChecksumFlag?4:0);

	bd = get8();
	blockSize = (bd>>4)&7;

	if (contentSizeFlag) {
		contentSize = get64();
	}
	if (dictionaryIdFlag) {
		dicId = get32();
	}
	hc = get8();

	toRead = len;
	wo.t = this;
	pthread_create(&thr,nullptr,ImportLZ4File::InternalThreadEntryFunc,&wo);

	return true;
}

[[maybe_unused]] void ImportLZ4File::dumpHeader() {
	cout <<  hex << "Magic number: 0x" << magic << dec << endl;
	cout << "Version " << (flg>>6) << endl;
	cout << "Block Independence flag " << blockIndependent << endl;
	cout << "Block checksum flag     " << blocChecksumFlag << endl;
	cout << "Content Size flag       " << contentSizeFlag << endl;
	cout << "Content checksum flag   " << contentChecksumFlag<< endl;
	cout << "Dictionary ID flag      " << dictionaryIdFlag << endl;
	cout << "Block Maximum Size      " << BlocSize[blockSize] << endl;
	cout << "flg" << (int)(flg) << " bd=" << (int)(bd) << " size="<<contentSize<<endl;
}

bool ImportLZ4File::fetch() {
	if (len == -1) {
		return true;
	}
	if (fileLoaded && cmpFrames.empty()) {	// EndMark + optional C. Checksum
		len = -1;
		return true;
	}

	uint32_t s = usefullBufferSize[workingBuffer]-pos;
	memcpy(buffers[1-workingBuffer],buffers[workingBuffer]+pos,s);
	workingBuffer = 1 - workingBuffer;

	while (s + BlocSize[blockSize] <= IMPORT_BUFFER_SIZE) {
		size_t listSize;
		listMu.lock();
		listSize = cmpFrames.size();
		listMu.unlock();
		while (listSize == 0) {
			usleep(2*1000);
			listMu.lock();
			listSize = cmpFrames.size();
			listMu.unlock();
		}
        int decBytes = LZ4_decompress_safe(&cmpFrames.front()[0],buffers[workingBuffer]+s, cmpFramesSize.front(), BlocSize[blockSize]);
		if (decBytes < 0) {
			Log::error(queryStatus, "Import file - Corrupted LZ4 file "+path+ " offset:"+to_string(decBytes));
			usefullBufferSize[workingBuffer] = 0;
			len = -1;
			close(fd);
		} else {
			len -= cmpFramesSize.front() + 4;
		}

		s += decBytes;
		listMu.lock();
		cmpFrames.pop_front();
		cmpFramesSize.pop_front();
		listMu.unlock();

		if (len <= endFrameSize) break;
		if (blocChecksumFlag) {
			get32();
		}
	}
	usefullBufferSize[workingBuffer] = s;

	return true;
}

void ImportLZ4File::processingLZ4Loop(WorkerLZ4 *) {
	while (toRead > endFrameSize ) {
		size_t listSize;
		uint32_t cmpBytes;
		size_t dummy = read(fd,&cmpBytes,4);
		toRead -= 4;

		listMu.lock();
		listSize = cmpFrames.size();
		listMu.unlock();

		while (listSize > 2*IMPORT_BUFFER_SIZE/(1<<22)) {
			usleep(20*1000);
			listMu.lock();
			listSize = cmpFrames.size();
			listMu.unlock();
		}
		std::vector<char> v;
		try {
			v.resize(cmpBytes);
		} catch (bad_alloc &e) {
			Log::error(queryStatus,"Out of memory in processingLZ4Loop");
			len = -1;
			break;
		}
		dummy = read(fd,&v[0],cmpBytes);
		toRead -= cmpBytes;
		listMu.lock();
		cmpFrames.push_back(v);
		cmpFramesSize.push_back(cmpBytes);
		listMu.unlock();
	}
	close(fd);
	fileLoaded = true;
}

}
