#pragma once

#include <cmath>

#include "baseIndex.hpp"
#include "../dictionary/dictionary.hpp"
#include "../common/float16.hpp"

namespace indexes {

class IndexChunk {
public:
	IndexChunk(std::shared_ptr<BaseIndex> &base,std::shared_ptr<dictionary::Dictionary> &dic,IndexChunkStructBase *data,IndexStructure *master, uint32_t chunk, double scale);
	void	addRef(const std::string_view &ref,bool isEndOfDocument) {
		uint32_t r = dic->addKey(ref);	// returns existing id if key already there
		base->add(r,false,isEndOfDocument);
		std::string g = std::string(ref.data(),ref.size());
	}
	void	addNull(bool isEndOfDocument) 				 	 {
	    base->add(0,true,isEndOfDocument);
	}
	//void	addBoolean(bool b,bool isEndOfDocument) 	 	 {	addUnsignedVal(b,isEndOfDocument);	}
	void	addSigned(const __int128_t &b,bool isEndOfDocument);
	void	addUnsigned(const __uint128_t &b,bool isEndOfDocument)  {  addUnsignedVal(b,isEndOfDocument); }
	void	addFloat16(float f,bool isEndOfDocument)	 	 {  addFloat16Val(f,floatToHalfFloat(f),isEndOfDocument); }
	void	addFloat(float f,bool isEndOfDocument)		 	 {  addFloatVal(f,floatToUint32(f),isEndOfDocument); }
	void	addDouble(double v,bool isEndOfDocument)	 	 {  addDoubleVal(v,doubleToUint64(v),isEndOfDocument); }
	void	addLocation(float x, float y,bool isEndOfDocument){ addGeopointVal(x,y,locationToUint64(x,y),isEndOfDocument); }

    [[maybe_unused]] void	addLocation(const __uint128_t &b,bool isEndOfDocument);
	void	addUnsignedScaled(double v,bool isEndOfDocument) {
        auto b = (__uint128_t)round(v*scale);
        const __uint128_t h = (((__uint128_t)1) << len)-1;
        b =  len < 128 ? std::min(b, h) : b;
		addUnsignedVal(b,isEndOfDocument);
	}
	void	addSignedScaled(double v,bool isEndOfDocument) {
        auto b = (__uint128_t)round(fabs(v)*scale);
        const __uint128_t h = (((__uint128_t)1) << len)-1;
        const __uint128_t o = (((__uint128_t)1) << (len-1))-1;
        __int128_t c = v < 0 ? (__int128_t)((-std::min(b, o))&h) : (__int128_t)std::min(b, o);
        addSigned(c,isEndOfDocument);
	}

	[[nodiscard]] BaseIndex* getBase() const {
		return base.get();
	}

	[[nodiscard]] std::shared_ptr<BaseIndex> getSharedBase() const {
	    return base;
	}

	[[nodiscard]] uint32_t getChunk() const {
		return chunk;
	}

	[[nodiscard]] bool isAquiredForWriting() const {
		return aquiredForWriting;
	}

	void setAquiredForWriting(bool _aquiredForWriting) {
		this->aquiredForWriting = _aquiredForWriting;
	}

    [[nodiscard]] std::shared_ptr<dictionary::Dictionary> getDictionary() const {
	    return dic;
	}

	[[nodiscard]] bool isFixed() const { return base->isFixed(); };

	void flush();
private:
    IndexChunkStructBase *data;
	IndexStructure *master;
	uint32_t		chunk;
	uint32_t        len;
	double			scale;
	std::shared_ptr<BaseIndex> base;
	std::shared_ptr<dictionary::Dictionary> dic;
	bool		    aquiredForWriting;
	double          minV,maxV;

	void 		addSignedVal(const __int128_t &val,bool isEndOfDocument);
	void 		addUnsignedVal(const __uint128_t &val,bool isEndOfDocument);
	void 		addFloat16Val(float val,uint64_t v, bool isEndOfDocument);
	void 		addFloatVal(float val,uint64_t v, bool isEndOfDocument);
	void 		addDoubleVal(double val,uint64_t v, bool isEndOfDocument);
	void 		addGeopointVal(float lat, float longi,uint64_t v, bool isEndOfDocument);

};
}
