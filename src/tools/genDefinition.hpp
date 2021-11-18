#pragma once

#include <iostream>
#include <memory>
#include <map>

#include "../queries/queryStatus.hpp"
#include "../mapping/mapping_types.hpp"
#include "../repository/importFile.hpp"

using namespace std;
using namespace repository;

namespace tools {

class GenDefinition : public ImportFile {
public:
    using ImportFile::ImportFile;
    std::string getDefinition(std::shared_ptr<queries::QueryStatus>& queryStatus);
private:
};
}
