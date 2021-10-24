#include "indexChunk.hpp"
#include "storageModel.hpp"

using namespace std;

namespace indexes {

inline double amin(uint32_t row,double val ,double a)  {
    return row == 0 ? val : min(val,a);
}

inline double amax(uint32_t row,double val ,double a)  {
    return row == 0 ? val : max(val,a);
}

IndexChunk::IndexChunk(shared_ptr<BaseIndex> &_base,shared_ptr<dictionary::Dictionary>&_dic,IndexChunkStructBase *_data,IndexStructure *_master, uint32_t _chunk, double _scale) :
		base(_base),
		dic(_dic),
		data(_data),
		master(_master),
		chunk(_chunk),
		scale(_scale),
		aquiredForWriting(false){
	data->chunk = _chunk;
	len = base->getLength();
	minV = master->minMax[chunk][0];
	maxV = master->minMax[chunk][1];
}

void IndexChunk::addUnsignedVal(const __uint128_t &val,bool isEndOfDocument) {
	minV = amin(base->getNbRows(),(double)val,minV);
	maxV = amax(base->getNbRows(),(double)val,maxV);
    base->add(val,false,isEndOfDocument);
}

void IndexChunk::addSignedVal(const __int128_t &val,bool isEndOfDocument) {
    minV = amin(base->getNbRows(),(double)val,minV);
    maxV = amax(base->getNbRows(),(double)val,maxV);
    base->add(val,false,isEndOfDocument);
}

void IndexChunk::addFloatVal(float v, uint64_t val,bool isEndOfDocument) {
	minV = amin(base->getNbRows(),minV,v);
	maxV = amax(base->getNbRows(),maxV,v);
    base->add(val,false,isEndOfDocument);
}

void IndexChunk::addFloat16Val(float v, uint64_t val,bool isEndOfDocument) {
    minV = amin(base->getNbRows(),minV,v);
    maxV = amax(base->getNbRows(),maxV,v);
    base->add(val,false,isEndOfDocument);
}

void IndexChunk::addDoubleVal(double v, uint64_t val,bool isEndOfDocument) {
    minV = amin(base->getNbRows(),minV,v);
    maxV = amax(base->getNbRows(),maxV,v);
    base->add(val,false,isEndOfDocument);
}

void IndexChunk::addGeopointVal(float lat, float longi, uint64_t val,bool isEndOfDocument) {
	auto *b = (float*)&minV;
	b[0] = (float)amin(base->getNbRows(),b[0],lat);
	b[1] = (float)amin(base->getNbRows(),b[1],longi);
	b =  (float*)&maxV;
	b[0] = (float)amax(base->getNbRows(),b[0],lat);
	b[1] = (float)amax(base->getNbRows(),b[1],longi);
    base->add(val,false,isEndOfDocument);
}

[[maybe_unused]] void IndexChunk::addLocation(const __uint128_t &v,bool isEndOfDocument) {
    float x,y;
    Uint64ToLocation((int64_t )v,x,y);
    addGeopointVal(x,y,v,isEndOfDocument);
}

void IndexChunk::addSigned(const __int128_t &b,bool isEndOfDocument)  	 {
    addSignedVal(signExtend(b,len),isEndOfDocument);
}

void IndexChunk::flush() {
    master->minMax[chunk][0] = minV;
    master->minMax[chunk][1] = maxV;
}

}
