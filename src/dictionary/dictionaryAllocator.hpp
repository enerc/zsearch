#pragma once

#include "dictionary.hpp"
#include "dictionaryManager.hpp"

#include <unordered_map>
#include <memory>

namespace dictionary {

class DictionaryAllocator {
public:
	static DictionaryAllocator* getInstance();
	std::shared_ptr<DictionaryManager> getManager(const std::string &_index,const std::string &_field,uint32_t maxArraySize);
	void sync();
	void removeEntry(const std::string &_index,const std::string &_field);
private:
	static DictionaryAllocator *instance;
	std::unordered_map<std::string,std::shared_ptr<DictionaryManager> > maps;
	std::mutex	mu;

	~DictionaryAllocator();
	static std::string getKey(const std::string &_index,const std::string &_field) ;
};

}
