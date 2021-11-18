#pragma once

#include <unistd.h>
#include <mutex>
#include <vector>
#include <list>
#include <thread>
#include "../repository/importTextFile.hpp"
#include "genDefinitionBase.hpp"

namespace tools {

class  GenDefinitionTextFile : public ImportTextFile, public GenDefinitionBase {
public:
    using ImportTextFile::ImportTextFile;
    std::string doGenImport() { return doBaseGenImport(this);}
private:
};

}
