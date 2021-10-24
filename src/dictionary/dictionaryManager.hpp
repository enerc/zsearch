#pragma once

#include <vector>
#include <thread>
#include <condition_variable>
#include <atomic>

#include "../common/config.hpp"
#include "dictionary.hpp"

namespace dictionary {

class DictionaryManager {
public:
	DictionaryManager(const std::string &_index,const std::string &_field, uint32_t maxArraySize);
	~DictionaryManager();

	void sync() ;
	void finish() ;
	std::shared_ptr<Dictionary> getDictionary(uint32_t chunk)  ;
	void 	removeDictionary() ;

    [[maybe_unused]] uint32_t getEntry(uint32_t chunk,const std::string &key,uint32_t crc32c) ;
    void    extendSpace(uint32_t chunks);
    void    dump(uint32_t chunk) ;
private:
	std::vector<std::shared_ptr<Dictionary>> dics;
	std::string 	index;
	std::string 	field;
	uint64_t 		*nbDictionary;
	uint64_t		*getIndexArray;
	int				fdhs;
	int				fdst;
    hash_storage	*hs;	// mmap hash
    strings_properties	*st;
    uint32_t        maxArraySize;
    static constexpr uint64_t chunkIncreaseSize = 1024;
    std::mutex	    mu;


	[[nodiscard]] uint64_t *getNumberOfDicFiles() const;
	[[nodiscard]] std::string getFileFolder() const;
	[[nodiscard]] std::string getKeyFileName() const;
	[[nodiscard]] std::string getHashFileName() const;
	[[nodiscard]] size_t getKeyInitialFileSize() const;
    [[nodiscard]] size_t getKeyFileSize() const;
	[[nodiscard]] size_t getKeyDataSize() const;
	[[nodiscard]] size_t getHashDataSize() const;
	[[nodiscard]] size_t getHashInitialFileSize() const;
    [[nodiscard]] size_t getHashFileSize() const;
	void createFiles();
    [[nodiscard]] hash_storage*	 getHashPointer(uint32_t chunk) const;
	[[nodiscard]] strings_properties* getKeyPointer(uint32_t chunk) const;
	void addChunkIfNecessary(uint32_t chunk);
};

}
