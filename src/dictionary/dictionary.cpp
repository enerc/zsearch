#include <cstring>

#include "dictionary.hpp"
#include "crc32c.hpp"

#define KEY_NOT_FOUND	0xffffffff
#define ENTRY_EMPTY		0xffffffff
#define ENTRY_LAST		0xfffffffe
#define DIC_SHIFT	    20
#define DIC_MASK	    0x0fffff
#define DIC_SIZE	    4096
#define FAST_LOOKUP     (8192*64-1)

using namespace std;

namespace dictionary {


bool Dictionary::fastlookup(uint32_t crc32c) const {
	crc32c &= FAST_LOOKUP;
	return _bextr_u64(hs->fastLookupHash[ crc32c >> 6],crc32c & 63,1);
}

void Dictionary::updateFastLookup(uint32_t crc32c) {
	crc32c &= FAST_LOOKUP;
    uint32_t l = crc32c >> 6;
    uint32_t p = crc32c & 63;
	hs->fastLookupHash[l] |= 1L << p;
}


uint32_t Dictionary::getIndex(uint32_t crc32c, const string_view &key, bool forceCheck) const {
    if (!fastlookup(crc32c)) {
        return KEY_NOT_FOUND;
    } else {
        uint32_t h = crc32c >> DIC_SHIFT;
        uint32_t l = crc32c & DIC_MASK;
        auto e = &hs->entries[h];
        while (true) {
            if (e->nextEntry == ENTRY_EMPTY) {
                return KEY_NOT_FOUND;
            }
            // Same crc32c hash
            if (e->lcrc == l) {
                if (e->hasCollision == 0U && !forceCheck) {
                    return e->value;
                } else {
                    // Check the string if we have a collision
                    uint32_t len = sizes[e->value + 1] - sizes[e->value];
                    bool eq = len == key.size();
                    uint8_t *base = strings + sizes[e->value];
                    for (unsigned int i = 0; i < len && eq; i++) {
                        if (base[i] != key.at(i)) {
                            eq = false;
                        }
                    }
                    if (eq) {
                        return e->value;
                    } else {
                        if (e->nextEntry == ENTRY_EMPTY || e->nextEntry == ENTRY_LAST) {
                            return KEY_NOT_FOUND;
                        }
                        e = &hs->extraEntries[e->nextEntry];
                    }
                }
            } else {
                if (e->nextEntry == ENTRY_LAST) {
                    return KEY_NOT_FOUND;
                }
                e = &hs->extraEntries[e->nextEntry];
            }
        }
    }
}

bool Dictionary::hasThisHash(uint32_t crc32c) const {
    uint32_t h = crc32c >> DIC_SHIFT;
    uint32_t l = crc32c & DIC_MASK;
    auto e = &hs->entries[h];
    if (e->nextEntry != ENTRY_EMPTY && e->lcrc == l) {
        return true;
    } else {
        if (e->nextEntry == ENTRY_EMPTY || e->nextEntry == ENTRY_LAST) {
            return false;
        } else {
            while (true) {
                e = &hs->extraEntries[e->nextEntry];
                if (e->lcrc == l) {
                    return true;
                } else if (e->nextEntry == ENTRY_LAST) {
                    return false;
                }
            }
        }
    }
}

void Dictionary::setDuplicate(uint32_t crc32c) {
    uint32_t h = crc32c >> DIC_SHIFT;
    uint32_t l = crc32c & DIC_MASK;
    auto e = &hs->entries[h];
    if (e->nextEntry == ENTRY_EMPTY) {
        return;
    }
    do {
        if (e->lcrc == l) {
            e->hasCollision = true;
        }
        if (e->nextEntry == ENTRY_LAST) {
            break;
        }
        e = &hs->extraEntries[e->nextEntry];
    } while (true);
    if (e->lcrc == l) {
        e->hasCollision = true;
    }
}

uint32_t Dictionary::addKey(const std::string_view &key) {
    uint32_t crc32c = getCrc32c(0, key.data(), key.size());
    uint32_t idx = getIndex(crc32c, key, true);
    if (idx != KEY_NOT_FOUND) {
        return idx;
    }
    // add the string
    sizes[hs->nbEntries + 1] = sizes[hs->nbEntries] + key.size();
    memcpy(strings + sizes[hs->nbEntries], key.data(), key.size());
    // insert the entry
    uint32_t h = crc32c >> DIC_SHIFT;
    uint32_t l = crc32c & DIC_MASK;
    auto e = &hs->entries[h];
    if (e->nextEntry == ENTRY_EMPTY) {
        // set entry on first level
        e->lcrc = l;
        e->hasCollision = 0;
        e->value = hs->nbEntries;
        e->nextEntry = ENTRY_LAST;
    } else {
        while (e->nextEntry != ENTRY_LAST) {
            e = &hs->extraEntries[e->nextEntry];
        }
        auto e2 = &hs->extraEntries[hs->nbExtraEntries];
        e2->lcrc = l;
        e2->hasCollision = hasThisHash(crc32c);
        if (e2->hasCollision) setDuplicate(crc32c);
        e2->nextEntry = ENTRY_LAST;
        e2->value = hs->nbEntries;
        e->nextEntry = hs->nbExtraEntries;
        hs->nbExtraEntries++;
    }
    hs->nbEntries++;
    updateFastLookup(crc32c);
    return hs->nbEntries - 1;
}

uint32_t Dictionary::getKey(const std::string &key) const {
    uint32_t crc32c = getCrc32c(0, key.c_str(), key.size());
    return getIndex(crc32c, key, true);
}

Dictionary::Dictionary(hash_storage *_h, strings_properties *_st, uint32_t _maxArraySize,bool create) {
	hs = _h;
	st = _st;
    maxArraySize = _maxArraySize;
    sizes = (uint32_t*)st;
	strings = (uint8_t*) (sizes + maxArraySize*CHUNK_SIZE);

	if (create) {
		for (auto &e : hs->entries) {
			e.nextEntry = ENTRY_EMPTY;
		}
		hs->nbEntries = 0;
		hs->nbExtraEntries = 0;
	}
}

string_view Dictionary::getString(uint32_t index) const {
    if ((int)sizes[index+1]-(int)sizes[index] > 0) {
        return {(const char *) strings + sizes[index], sizes[index + 1] - sizes[index]};
    } else {
        return {};
    }
}

void Dictionary::dump() {
	cout << "Dump dictionary: " << DIC_SIZE << " entries" << endl;

	//cout << dec << sizeof(T) << endl;
#if 1
    for (uint32_t i=0; i< DIC_SIZE; i++) {
		entry *e = &hs->entries[i];
		if (e->nextEntry != ENTRY_EMPTY) {
			uint32_t m = i << DIC_SHIFT | e->lcrc;
			cout << hex << "Hash " << m << " " << dec << "Index:" << e->value << " Duplicate:" << e->hasCollision << " " ;
			auto sz = sizes[e->value + 1] - sizes[e->value];
			cout << " size="<<sz << " " ;
			uint8_t *base = strings+sizes[e->value];
			for (uint32_t j=0; j < sz; j++) cout << base[j];
			cout << endl;
			while (e->nextEntry != ENTRY_EMPTY && e->nextEntry != ENTRY_LAST) {
				e= &hs->extraEntries[e->nextEntry];
				m = i << DIC_SHIFT | e->lcrc;
				cout << hex << "Hash " << m << " " << dec << "Index:" << e->value << " Duplicate:" << e->hasCollision << " ";
				sz = sizes[e->value + 1] - sizes[e->value];
                cout << " size="<<sz << " " ;
				base = strings+sizes[e->value];
				for (uint32_t j=0; j < sz; j++) cout << base[j];
				cout << endl;
			}
		}
	}
	cout << endl;
#endif
	//for (int i=0; i < 512; i++)  cout << hex << hs->fastLookupHash[i] << " ";
}

}
