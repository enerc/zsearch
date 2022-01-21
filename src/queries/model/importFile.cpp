#include "importFile.hpp"
#include "../builders/ast.hpp"

using namespace repository;
using namespace std;
using namespace mapping;

namespace queries::model {

uint64_t ImportFileModel::execShaderOnCpu() {
    ImportFile ifile(queryStatus,path,indexName);
    if (queryStatus->status != HTTP_200) return false;
    auto ret = ifile.doImport(queryStatus);
    string s = to_string(queryStatus->processedDocuments) + " processed "+to_string(queryStatus->failedDocuments)+" failed " + to_string(queryStatus->warnings) +" warnings " + to_string(queryStatus->errors) + " errors" ;
    queryStatus->customMessage = s;
    queryStatus->status = HTTP_204;
    return ret;
}

[[maybe_unused]] void ImportFileModel::setQueryStatus(const std::shared_ptr<queries::QueryStatus> &q) {
       ImportFileModel::queryStatus = q;
}

void ImportFileModel::execute(queries::builders::AST *ast) {
    queryStatus = ast->getQueryStatus();
    shared_ptr<IndexDefinition> id = IndexDefinitionManager::getInstance()->getIndex(indexName);
    if (id == nullptr) {
        Log::error(queryStatus,"Index not found " + indexName);
    } else {
        execShaderOnCpu();
    }
}

}
