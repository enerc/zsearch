#include "dictionaryManager.hpp"
#include "dictionaryAllocator.hpp"
#include "crc32c.hpp"

#include <algorithm>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <cstring>

using namespace std;

constexpr int MANAGER_RESERVED = 4096;

namespace dictionary {

DictionaryManager::DictionaryManager(const std::string &_index,const std::string &_field,uint32_t _maxArraySize) {
	index= _index;
	field = _field;
	getIndexArray = nullptr;
    maxArraySize = _maxArraySize;

	// open files
	bool created = false;
	fdhs =  open(getHashFileName().c_str(), O_RDWR, (mode_t)0600);
	if (fdhs < 0) {
		createFiles();
		created = true;
		fdhs =  open(getHashFileName().c_str(), O_RDWR, (mode_t)0600);
		if (fdhs < 0) {
			Log::panic("Cannot open file "+getHashFileName());
		}
	}
	fdst =  open(getKeyFileName().c_str(), O_RDWR, (mode_t)0600);
	if (fdst < 0) {
		Log::panic("Cannot open file "+getKeyFileName());
	}

	// Map files
	hs = (hash_storage*)mmap(nullptr, getHashFileSize(), PROT_READ|PROT_WRITE, MAP_SHARED, fdhs, 0);
	if (hs == MAP_FAILED) {
		Log::panic("Failed to memory map "+getHashFileName()+" size:"+to_string(getHashFileSize()));
	}
	st = (strings_properties*)mmap(nullptr, getKeyFileSize(), PROT_READ|PROT_WRITE, MAP_SHARED, fdst, 0);
	if (st == MAP_FAILED) {
		Log::panic("Failed to memory map "+getKeyFileName()+" size:"+to_string(getKeyFileSize()));
	}

	if (created) {
		Log::debug("Dictionary " + getKeyFileName() + " created");

		nbDictionary = getNumberOfDicFiles();
		*nbDictionary = 0;
	}
	nbDictionary = getNumberOfDicFiles();
	Log::debug("dictionary "+_index+"."+field+" opened . "+to_string(*nbDictionary)+" chunks");

	// map objects to memory mapped files
	for (uint32_t i = 0; i < *nbDictionary; i++) {
		shared_ptr<Dictionary> a(new Dictionary(getHashPointer(i),getKeyPointer(i),maxArraySize));
		dics.push_back(a);
	}
}

DictionaryManager::~DictionaryManager() {
	munmap(hs,getHashFileSize());
    close(fdhs);
	munmap(st,getKeyFileSize());
    close(fdst);

	free(getIndexArray);
	getIndexArray = nullptr;
	hs = nullptr;
	st = nullptr;

	dics.clear();
}

void DictionaryManager::finish() {
}

shared_ptr<Dictionary> DictionaryManager::getDictionary(uint32_t chunk) {
    shared_ptr<Dictionary> ret;
    mu.lock();
    for (uint32_t p = dics.size() ; p <= chunk; p++) {
        addChunkIfNecessary(p);
    }
    ret = dics.at(chunk);
    mu.unlock();
    return ret;
}

// inside lock/unlock
void DictionaryManager::addChunkIfNecessary(uint32_t i) {
	memset(getHashPointer(i),0,10);
	shared_ptr<Dictionary> a(new Dictionary(getHashPointer(i),getKeyPointer(i),maxArraySize,true));
	dics.push_back(a);
	(*nbDictionary) ++;
}

void DictionaryManager::sync() {
	msync(hs,getHashFileSize(),MS_SYNC);
	msync(st,getKeyFileSize(),MS_SYNC);
}

inline bool ends_with(std::string const & value, std::string const & ending){
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

string DictionaryManager::getFileFolder() const {
	return  Config::getInstance()->getDictionaryPath() + "/" +  index + ".";
}

string DictionaryManager::getHashFileName() const {
	string s = getFileFolder() + field + ".hash";
	return s;
}

string DictionaryManager::getKeyFileName() const {
	string s = getFileFolder() + field + ".key";
	return s;
}

size_t DictionaryManager::getHashDataSize() const {
	return sizeof(hash_storage) + sizeof(entry)*CHUNK_SIZE*maxArraySize;
}

size_t DictionaryManager::getHashInitialFileSize() const {
	return MANAGER_RESERVED + 2*chunkIncreaseSize*getHashDataSize();
}

size_t DictionaryManager::getHashFileSize() const {
    return getMapSize(fdhs);
}

size_t DictionaryManager::getKeyDataSize() const {
	return CHUNK_SIZE*(sizeof(uint32_t)+AVERAGE_BYTES_PER_STRING)*maxArraySize;
}

size_t DictionaryManager::getKeyInitialFileSize() const {
	return 2*chunkIncreaseSize*getKeyDataSize();
}

size_t DictionaryManager::getKeyFileSize() const {
    return getMapSize(fdst);
}

hash_storage* DictionaryManager::getHashPointer(uint32_t chunk) const {
	uint64_t offset = MANAGER_RESERVED + chunk*getHashDataSize();
	uint64_t offset2 = MANAGER_RESERVED + (chunk+1)*getHashDataSize();
	if (offset2 > getHashFileSize()) {
		Log::error("In index:"+index+" field:"+field+". Storage capacity exceeded. ");
		offset = MANAGER_RESERVED;
	}
	char *t = (char *)hs;
	return (hash_storage*)(t + offset);
}

strings_properties* DictionaryManager::getKeyPointer(uint32_t chunk) const {
	uint64_t offset = MANAGER_RESERVED + chunk * getKeyDataSize();
	char *t = (char *)st;
	return (strings_properties*)(t + offset);
}

uint64_t *DictionaryManager::getNumberOfDicFiles() const {
	if (hs == nullptr) {
		return nullptr;
	} else {
		return (uint64_t*)hs;
	}
}
// Create sparse files
void DictionaryManager::createFiles() {
	string h = getHashFileName();
	fdhs = open(h.c_str(), O_WRONLY | O_CREAT, (mode_t)0600);
	if (fdhs < 0) {
		Log::error("Failed to create file "+h);
	}
	size_t fileSize = getHashInitialFileSize();
	lseek(fdhs,(off_t)fileSize,SEEK_CUR);
	if (write(fdhs,"\0",1) < 0) {
		Log::error("fail to create "+h);
	}
	close(fdhs);

	h = getKeyFileName();
	fdst = open(h.c_str(), O_WRONLY | O_CREAT, (mode_t)0600);
	if (fdst < 0) {
		Log::error("Failed to create file "+h);
	}
	fileSize = getKeyInitialFileSize();
	lseek(fdst,(off_t)fileSize,SEEK_CUR);
	if (write(fdst,"\0",1) < 0) {
		Log::error("Failed to create file "+h);
	}
	close(fdst);
}

void DictionaryManager::removeDictionary() {
    mu.lock();
	dics.clear();
	remove(getHashFileName().c_str());
	remove(getKeyFileName().c_str());
	DictionaryAllocator::getInstance()->removeEntry(index,field);
	mu.unlock();
}

    [[maybe_unused]] uint32_t DictionaryManager::getEntry(uint32_t chunk,const std::string &key,uint32_t crc32c) {
    return dics.at(chunk)->getIndex(crc32c,key,false);
}

void DictionaryManager::dump(uint32_t chunk) {
    dics.at(chunk)->dump();
}

void DictionaryManager::extendSpace(uint32_t chunks) {
    mu.lock();
    hs = (hash_storage*)extendMapping(fdhs,chunks*getHashDataSize(),(char*)hs);
    st = (strings_properties*)extendMapping(fdst,chunks*getKeyDataSize(),(char*)st);
    dics.clear();
    nbDictionary = getNumberOfDicFiles();
    for (uint32_t i = 0; i < *nbDictionary; i++) {
        shared_ptr<Dictionary> a(new Dictionary(getHashPointer(i),getKeyPointer(i),maxArraySize));
        dics.push_back(a);
    }
    mu.unlock();
}

}



