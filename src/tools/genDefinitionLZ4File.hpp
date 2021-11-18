#pragma once

#include <unistd.h>
#include <mutex>
#include <vector>
#include <list>
#include <thread>
#include "../repository/importLZ4File.hpp"
#include "genDefinitionBase.hpp"

namespace tools {

class  GenDefinitionLZ4File : public ImportLZ4File, public GenDefinitionBase {
public:
    using ImportLZ4File::ImportLZ4File;
    std::string doGenImport() { return doBaseGenImport(this);}
private:

};

}
