#include "indexDefinitionManager.hpp"
#include "../common/tools.hpp"

using namespace std;

namespace mapping {


IndexDefinitionManager* IndexDefinitionManager::instance = nullptr;

IndexDefinitionManager* IndexDefinitionManager::getInstance() {
	if (instance == nullptr)  {
		instance = new IndexDefinitionManager();
	}
	return instance;
}

std::shared_ptr<IndexDefinition> IndexDefinitionManager::getIndex(const string_view &index) {
    string s(index.data(),index.size());
    std::shared_ptr<IndexDefinition> ret = nullptr;
    mu.lock();
	auto o = indexDefinitionMap.find(s);
	if (o != indexDefinitionMap.end()) {
		ret = o->second;
	}
	mu.unlock();
	if (ret == nullptr) {
		ret = make_shared<IndexDefinition>(s);
		if (ret->fromDisk(false)) {
		    mu.lock();
		    indexDefinitionMap[s] = ret;
		    mu.unlock();
		    ret->updateIndexRef();
		} else {
		    ret = nullptr;
		}
	}
	return ret;
}

std::shared_ptr<IndexDefinition> IndexDefinitionManager::createIndex(const string_view &index) {
    string s(index.data(),index.size());
    std::shared_ptr<IndexDefinition> ret;
    mu.lock();
    auto o = indexDefinitionMap.find(s);
    if (o != indexDefinitionMap.end()) {
        ret = o->second;
    }
    mu.unlock();
    if(ret == nullptr) {
        ret = make_shared<IndexDefinition>(s);
        mu.lock();
        indexDefinitionMap[s] = ret;
        mu.unlock();
    }
    return ret;
}

std::shared_ptr<mapping::IndexDefinition> IndexDefinitionManager::getIndexManager(const string_view &s, const vector<string>&indexes,shared_ptr<queries::QueryStatus>& queryStatus) {
	std::shared_ptr<mapping::IndexDefinition> ret;

	if (s.find('.') != string::npos) {
		auto h = s.substr(0, s.find('.'));
		auto m = s.substr(s.find('.')+1);
		shared_ptr<mapping::IndexDefinition> i = IndexDefinitionManager::getInstance()->getIndex(h);
		if (i != nullptr) {
			if (i->hasEntry(m)) {
				return i;
			} else {
				Log::error(queryStatus,string("Unknown column ")+string(m.data(),m.size()) + " in table " + string(h.data(),h.size()));
			}
		} else {
			for (const auto &j : indexes) {
				shared_ptr<mapping::IndexDefinition> o = IndexDefinitionManager::getInstance()->getIndex(j);
				if (o == nullptr) {
					Log::error(queryStatus,string("Unknown table ")+string(j.data(),j.size()));
				} else {
					auto n = s.substr(s.find('.'));
					if (o->hasEntry(n))
						return o;
				}
			}
			Log::error(queryStatus,string("Unknown column ")+string(h.data(),h.size()));
		}
	} else {
		for (const auto &i : indexes) {
            shared_ptr<mapping::IndexDefinition> o = IndexDefinitionManager::getInstance()->getIndex(i);
            if (o == nullptr) {
				Log::error(queryStatus,string("Unknown table ")+string(i.data(),i.size()));
			} else {
				if (o->hasEntry(s)) {
                    return o;
                }
			}
		}
	}

	return nullptr;
}

bool IndexDefinitionManager::removeIndex(const string_view &index) {
    string s(index.data(),index.size());
    bool ret;
	std::shared_ptr<IndexDefinition> p = getIndex(s);
	mu.lock();
	if (p != nullptr) {
		auto o = indexDefinitionMap.find(s);
		indexDefinitionMap.erase(o);
		p->removeIndex();
		ret = true;
	} else {
	    ret = false;
	}
	mu.unlock();
	return ret;
}

bool IndexDefinitionManager::truncateIndex(const string_view &index) {
    string s(index.data(),index.size());
    bool ret;
    std::shared_ptr<IndexDefinition> p = getIndex(s);
    mu.lock();
    if (p != nullptr) {
        auto o = indexDefinitionMap.find(s);
        indexDefinitionMap.erase(o);
        p->truncateIndex();
        ret = true;
    } else {
        ret = false;
    }
    mu.unlock();
    return ret;
}
}
