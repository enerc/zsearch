#pragma once

#include <utility>
#include <vector>

#include "importFileBase.hpp"
#include "../queries/queryStatus.hpp"

namespace repository {


enum FileType {
	FileTypeText,
	FileTypeLZ4
};

class ImportFile {
public:
	ImportFile(std::shared_ptr<queries::QueryStatus>& queryStatus,std::string _path,std::string _index)  {
	    path = std::move(_path);
	    index = std::move(_index);
        fileType = FileTypeText;
		getFileType(queryStatus);
	}
	bool	doImport(std::shared_ptr<queries::QueryStatus>& queryStatus);
private:
	std::string path;
	std::string index;
	FileType fileType;

	bool  getFileType(std::shared_ptr<queries::QueryStatus>& queryStatus);
};
}
