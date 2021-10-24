#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include "../queries/queryStatus.hpp"

typedef enum {
	error_none,
	error_parse
} error_type;

typedef enum {
    fatalOut = 0,
    errorOut = 1,
    warnOut  = 2,
    infoOut  = 3,
    debugOut = 4
} LogOutput;

extern bool verbose_terminal;

class Log {
public:
	static void init();
	static void debug(const std::string &msg);
	static void info(const std::string &msg);
	static void warn(const std::string &msg);
	static void error(const std::string &msg);
	static void error(std::shared_ptr<queries::QueryStatus> &queryStatus,const std::string &msg);
	static void panic(const std::string &msg);
private:
	Log() = default;
	static std::string timestamp();

    static LogOutput logOuputLevel;
};
