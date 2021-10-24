#include "../common/log.hpp"
#include "importFile.hpp"
#include "../common/tools.hpp"
#include "importTextFile.hpp"
#include "importLZ4File.hpp"

#include <fcntl.h>
#include <unistd.h>

using namespace std;

namespace repository {

bool ImportFile::getFileType(std::shared_ptr<queries::QueryStatus>& queryStatus) {
	int fd = open(path.c_str(),O_RDONLY);
	if (fd < 0 ) {
    	Log::error(queryStatus, "Import file FAILED for file "+path+ " cannot open this file");
    	return false;
	}
	uint32_t magic = 0;
	size_t e = read(fd,&magic, 4);
	if (e < 0) {
    	Log::error(queryStatus, "Import file FAILED for file "+path+ " cannot read this file");
    	close(fd);
    	return false;
	}
	close(fd);

	fileType = FileTypeText;

	switch (magic) {
        case 0x184d2204:
            fileType = FileTypeLZ4;
            break;

        default:
            // supposed to be uncompressed JSON
            break;
    }
	return true;
}

bool ImportFile::doImport(std::shared_ptr<queries::QueryStatus>& queryStatus) {
	if (fileType == FileTypeText) {
		ImportTextFile a(path,index,queryStatus);
		return a.doImport();
	} else if (fileType == FileTypeLZ4) {
		ImportLZ4File a(path,index,queryStatus);
		return a.doImport();
	}
	return false;
}

}
