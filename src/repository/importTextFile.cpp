#include "importTextFile.hpp"
#include "../../config.h"

#include <fcntl.h>
#include <unistd.h>

using namespace std;

namespace repository {

bool ImportTextFile::fetch() {
	if (len == -1) {
		return true;
	}
	if (position >= len-1) {
		close(fd);
		len = -1;	// eof
		return true;
	}

	long adjustedPosition = (long)(position & (~O_DIRECT_ALIGN));
	toSkip = position - adjustedPosition;
	lseek(fd,adjustedPosition,SEEK_SET);
	position = adjustedPosition;

	size_t sizeToRead = (len-position) < IMPORT_BUFFER_SIZE ? (len-position) : IMPORT_BUFFER_SIZE;
	if (sizeToRead != IMPORT_BUFFER_SIZE) {
		close(fd);
		fd =  open(path.c_str(), O_RDONLY);
		if (fd < 0 ) {
	    	Log::error("Import file FAILED for file "+path+ " cannot open this file");
	    	return false;
		}
		lseek(fd,adjustedPosition,SEEK_SET);
	}
	workingBuffer = 1 - workingBuffer;
	size_t o = read(fd,buffers[workingBuffer],sizeToRead);
	if (o != sizeToRead) {
    	Log::error("Import file FAILED for file "+path+ " broken file (error:"+to_string(errno)+")");
    	return false;
	}
	usefullBufferSize[workingBuffer] = o;
	return true;
}

}
