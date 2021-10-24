#pragma once

#include "importFileBase.hpp"

namespace repository {

class ImportTextFile : public ImportFileBase {
public:
	using ImportFileBase::ImportFileBase;
	bool  fetch() override;
	bool  decodeHeader() override { return true;}
private:
};
}
