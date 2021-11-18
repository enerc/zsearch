#include "genDefinition.hpp"
#include "genDefinitionLZ4File.hpp"
#include "genDefinitionTextFile.hpp"

using namespace std;


namespace tools {

string GenDefinition::getDefinition(std::shared_ptr<queries::QueryStatus>& queryStatus) {
    if (fileType == FileTypeText) {
        GenDefinitionTextFile a(path,index,queryStatus);
        return a.doGenImport();
    } else if (fileType == FileTypeLZ4) {
        GenDefinitionLZ4File a(path,index,queryStatus);
        return a.doGenImport();
    }
    Log::error(queryStatus,"File type non recognized.");
    return {};
}

}