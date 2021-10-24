#pragma once
#include <cinttypes>
#include <iomanip>
#define HAS_UNCAUGHT_EXCEPTIONS 1
#include "howardDate.h"

uint64_t stringToDate(bool isFt,bool isNano,const std::string &format, const std::string_view &v, bool &fail) ;
