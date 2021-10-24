#include "dictionaryAllocator.hpp"

using namespace std;

namespace dictionary {

DictionaryAllocator* DictionaryAllocator::instance = nullptr;

DictionaryAllocator* DictionaryAllocator::getInstance() {
	if (instance == nullptr)  {
		instance = new DictionaryAllocator();
	}
	return instance;
}

string DictionaryAllocator::getKey(const std::string &_index,const std::string &_field) {
	return _index+"."+_field;
}

shared_ptr<DictionaryManager> DictionaryAllocator::getManager(const std::string &_index,const std::string &_field, uint32_t maxArraySize) {
	const std::string s = getKey(_index,_field);
	auto o = maps.find(s);
	if (o != maps.end()) {
		return o->second;
	} else {
		shared_ptr<DictionaryManager> p = make_shared<DictionaryManager>(_index,_field,maxArraySize);
		maps.insert(std::make_pair(s,p));
		o = maps.find(s);
		return o->second;
	}
}

void DictionaryAllocator::sync() {
	for (const auto& o : maps) {
		o.second->sync();
	}
}

DictionaryAllocator::~DictionaryAllocator() {
	sync();
}

void DictionaryAllocator::removeEntry(const string &_index, const string &_field) {
    mu.lock();
    const std::string &s = getKey(_index,_field);
    auto o = maps.find(s);
    if (o != maps.end()) {
        maps.erase(o);
    }
    mu.unlock();
}

}
