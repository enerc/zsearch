#include "indexDefinition.hpp"

#include <iomanip>
#include <filesystem>
#include <cstring>
#include <cstdio>
#include "../repository/indexRepository.hpp"

using namespace std;
using namespace indexes;

constexpr int max_key_size = 65536;

namespace mapping {

IndexDefinition::IndexDefinition(const std::string_view &_indexName) {
	indexName = _indexName;
    std::vector<string_view> s;
    double gammaFactor = 100.0;
    bphf = new boomphf::mphf<std::string_view,Custom_string_Hasher>(s.size(),s,1,gammaFactor);
}

IndexDefinition::~IndexDefinition() {
	delete bphf;
}

void IndexDefinition::addEntry(const string_view &key, const mapping_entry &e) {
	if (hasEntryNoBoo(key)) {
		Log::error("IndexDefinition::createEntry called but entry already exists");
		return;
	}
    keys.emplace_back(key.data(),key.size());
    mae.push_back(e);
}

void IndexDefinition::updateEntry(const string_view &key, const mapping_entry &e, bool force) {
	if (!hasEntryNoBoo(key)) {
        addEntry(key,e);
		return;
	}
    uint32_t p = 0;
    for (auto &n : keys) {
        if (n == key) break;
        p++;
    }

	if (mae[p].s != e.s && !force) {
		Log::error("Index storage type cannot be changed");
		return;
	}
	if (mae[p].length != e.length && !force) {
		Log::error("Index bit length cannot be changed");
		return;
	}
    mae[p] = e;
}

void IndexDefinition::dump() const {
    cout << endl << "DUMP" << endl;
    int o = 0;
	for (const auto& t : mae) {
	    cout << " ---- " << keys[o];
		cout << " flag:" << hex << " " << t.flags;
		if ((t.flags&EntryStorageTypeSet) != 0) cout << " StorageType:" << getStorageName(t.s);
		if ((t.flags&EntryIndexTypeSet) != 0)   cout << " IndexType:" << getIndexTypeName(t.t,t.length);
		if ((t.flags&EntryArraySet) != 0) cout << " array max size " << t.max_array_size;
		if ((t.flags&EntryFormatSet) != 0)   cout << " Format:" << t.format;
        if ((t.flags&EntryJoinSet) != 0)   cout << " Join " << t.joinIndex << ":" << t.joinThisKey << " " << " " << t.joinRemoteKey << " " << t.joinValue;
		cout << endl;
		o++;
	}
}

string IndexDefinition::getFileName() const {
	return Config::getInstance()->getDictionaryPath() + "/" + string(indexName) + ".def";
}

void IndexDefinition::toDisk() const {
	auto n = getFileName();

	// create dictionary folder if it does not exists
	std::filesystem::path p = Config::getInstance()->getDictionaryPath();
	if (!filesystem::is_directory(p) || !filesystem::exists(p)) { // Check if src folder exists
		filesystem::create_directory(p); // create data folder
	}
	FILE *f = fopen(n.c_str(),"w");
	if (f == nullptr) {
		Log::panic("Cannot open index definition for writing. Path="+n);
	}
	uint32_t a;
	a = keys.size();
	fwrite(&a,sizeof(uint32_t),1,f);
	int k = 0;
	for (const auto& i : keys) {
		a = i.size();
		fwrite(&a,sizeof(uint32_t),1,f);
		fwrite(i.data(),a,1,f);
		const mapping_entry &j = mae[k];
		a = (uint32_t) j.flags;
		fwrite(&a,sizeof(uint32_t),1,f);
		a = (uint32_t) j.length;
		fwrite(&a,sizeof(uint32_t),1,f);
		a = (uint32_t)j.s;
		fwrite(&a,sizeof(uint32_t),1,f);
		a = (uint32_t)j.t;
		fwrite(&a,sizeof(uint32_t),1,f);
		a = (uint32_t)j.scale;
		fwrite(&a,sizeof(uint32_t),1,f);
		a = (uint32_t)j.max_array_size;
		fwrite(&a,sizeof(uint32_t),1,f);
		a = (uint32_t)j.format.size();
		fwrite(&a,sizeof(uint32_t),1,f);
		if (a > 0)
			fwrite(j.format.c_str(),a,1,f);
        a = (uint32_t)j.joinIndex.size();
        fwrite(&a,sizeof(uint32_t),1,f);
        if (a > 0)
            fwrite(j.joinIndex.c_str(),a,1,f);
        a = (uint32_t)j.joinThisKey.size();
        fwrite(&a,sizeof(uint32_t),1,f);
        if (a > 0)
            fwrite(j.joinThisKey.c_str(),a,1,f);
        a = (uint32_t)j.joinRemoteKey.size();
        fwrite(&a,sizeof(uint32_t),1,f);
        if (a > 0)
            fwrite(j.joinRemoteKey.c_str(),a,1,f);
        a = (uint32_t)j.joinValue.size();
        fwrite(&a,sizeof(uint32_t),1,f);
        if (a > 0)
            fwrite(j.joinValue.c_str(),a,1,f);

        k++;
	}
	fclose(f);
}

bool IndexDefinition::fromDisk(bool update) {
	auto n = getFileName();
	FILE *f = fopen(n.data(),"r");
	if (f == nullptr) {
	    Log::debug("Index definition " +indexName + " not on disk. Path="+n);
		return false;
	}
	uint32_t s;
	uint32_t a;
	char *tmp = (char*)malloc(max_key_size);
	size_t r;

	r = fread(&s,sizeof(uint32_t),1,f);
    keys.resize(s);
    mae.resize(s);
	for (int i=0; i< s; i++) {
		r |= fread(&a,sizeof(uint32_t),1,f);
		r |= fread(tmp,a,1,f);
		string k(tmp,a);
		keys.at(i) = k;

		mapping_entry e;
		e.me = k;
		r |= fread(&a,sizeof(uint32_t),1,f);
		e.flags = a;
		r |= fread(&a,sizeof(uint32_t),1,f);
		e.length = a;
		r |= fread(&a,sizeof(uint32_t),1,f);
		e.s = (mapping::storage_types)a;
		r |= fread(&a,sizeof(uint32_t),1,f);
		e.t =(mapping::index_types) a;
		r |= fread(&a,sizeof(uint32_t),1,f);
		e.scale = a;
		r |= fread(&a,sizeof(uint32_t),1,f);
		e.max_array_size = a;
		r |= fread(&a,sizeof(uint32_t),1,f);
		if (a>0) {
			r = fread(tmp,a,1,f);
			e.format = string(tmp,a);
		}
        r |= fread(&a,sizeof(uint32_t),1,f);
        if (a>0) {
            r = fread(tmp,a,1,f);
            e.joinIndex = string(tmp,a);
        }
        r |= fread(&a,sizeof(uint32_t),1,f);
        if (a>0) {
            r = fread(tmp,a,1,f);
            e.joinThisKey = string(tmp,a);
        }
        r |= fread(&a,sizeof(uint32_t),1,f);
        if (a>0) {
            r = fread(tmp,a,1,f);
            e.joinRemoteKey = string(tmp,a);
        }
        r |= fread(&a,sizeof(uint32_t),1,f);
        if (a>0) {
            r = fread(tmp,a,1,f);
            e.joinValue = string(tmp,a);
        }
		mae.at(i) = e;
	}
	fclose(f);
	free(tmp);

	if (update) {
        updateIndexRef();
    }

	// for compiler warning
	if (r == 0) {
	    return true;
	}
	return true;
}

void IndexDefinition::updateIndexRef() {
    std::vector<string_view> s;
    for (const auto& t : keys) {
        s.push_back(t);
    }
    double gammaFactor = 1000.0;// should avoid false match
    delete bphf;
    bphf = new boomphf::mphf<std::string_view,Custom_string_Hasher>(s.size(),s,1,gammaFactor);

    mae_boo.resize(keys.size());
    idxs.resize(keys.size());
    int i = 0;

	for (auto &t : keys) {
        uint64_t h = bphf->lookup(t);
        idxs.at((uint32_t)h) = indexes::IndexRepository::getInstance()->getIndex(indexName,t,mae.at(i));
        mae_boo.at((uint32_t)h) = mae.at(i);
        i++;
	}
}


void IndexDefinition::removeIndex() {
	for (auto &e : idxs) {
		IndexRepository::getInstance()->removeIndex(e->getIndex(),e->getHash());
	}
	auto n = getFileName();
	remove(n.data());
	keys.clear();
	mae.clear();
	idxs.clear();
}

void IndexDefinition::truncateIndex() {
    for (auto &e : idxs) {
        IndexRepository::getInstance()->removeIndex(e->getIndex(),e->getHash());
    }
    idxs.clear();
}

storage_types IndexDefinition::getStorageType(const std::string_view &key) const {
	if  (getIndex(key) != nullptr) return getIndex(key)->getStorageType();
	else return storage_type_null;
}

[[maybe_unused]] index_types IndexDefinition::getIndexType(const std::string_view &key) const {
    uint64_t h = bphf->lookup(key);
    if (h == ULLONG_MAX) {
        return index_type_none;
    } else {
        return mae_boo.at((uint32_t)h).t;
    }
}

double IndexDefinition::getScale(const std::string &key) const {
    uint64_t h = bphf->lookup(key);
    if (h == ULLONG_MAX) {
        return 1;
    } else {
        return pow(10.0,mae_boo.at((uint32_t)h).scale);
    }
}

[[maybe_unused]] uint32_t IndexDefinition::getLength(const std::string &key) const {
    uint64_t h = bphf->lookup(key);
    if (h == ULLONG_MAX) {
        return 1;
    } else {
        return mae_boo.at((uint32_t)h).length;
    }
}

[[maybe_unused]] bool IndexDefinition::isJoin(const std::string_view &key) const {
    uint64_t h = bphf->lookup(key);
    if (h == ULLONG_MAX) {
        return false;
    } else {
        return (mae_boo.at((uint32_t)h).flags & EntryJoinSet ) != 0;
    }
}

}
