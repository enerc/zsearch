#include <iostream>
#include <memory>

#include "log.hpp"
#include "config.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <iomanip>

using namespace std;
using namespace spdlog;

#define TERMINAL_COLOR_DEBUG "\033[38;5;28m"
#define TERMINAL_COLOR_INFO  "\033[38;5;46m"
#define TERMINAL_COLOR_WARN  "\033[38;5;226m"
#define TERMINAL_COLOR_ERROR "\033[38;5;196m"
#define TERMINAL_COLOR_FATAL "\033[38;5;9m"
#define TERMINAL_COLOR_WHITE "\033[0m"

static std::shared_ptr<spdlog::logger> rotating_logger;
LogOutput Log::logOuputLevel;
bool verbose_terminal = false;

static bool icompare_pred(unsigned char a, unsigned char b) {
    return tolower(a) == tolower(b);
}

static bool icompare(std::string const& a, std::string const& b) {
    if (a.length()==b.length()) {
        return std::equal(b.begin(), b.end(), a.begin(), icompare_pred);
    } else {
        return false;
    }
}

string Log::timestamp() {
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    std::ostringstream oss;
    oss << "[" << setfill('0') << setw(2)  << now->tm_hour << ":" << setw(2)  << now->tm_min << ":"  << setw(2)  << now->tm_sec << "] ";
    return oss.str();
}

void Log::init() {
	Config *config = Config::getInstance();
	rotating_logger = spdlog::rotating_logger_mt("SearchLog", config->getLogPath(),config->getMaxLogFileSize(), config->getMaxRotatedFiles());

	rotating_logger->set_level(spdlog::level::debug);
	rotating_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
    spdlog::flush_every(std::chrono::seconds(5));
    logOuputLevel = errorOut;

	if (icompare(config->getLogLevel(),"DEBUG")) {
		rotating_logger->set_level(spdlog::level::debug);
        logOuputLevel = debugOut;
	}
	if (icompare(config->getLogLevel(),"INFO")) {
		rotating_logger->set_level(spdlog::level::info);
        logOuputLevel = infoOut;
	}
	if (icompare(config->getLogLevel(),"WARNING")) {
		rotating_logger->set_level(spdlog::level::warn);
        logOuputLevel = warnOut;
	}
	if (icompare(config->getLogLevel(),"ERROR")) {
	    rotating_logger->set_level(spdlog::level::err);
	    logOuputLevel = errorOut;
	}
}

void Log::debug(const std::string &msg) {
	rotating_logger->debug(msg);
	if (verbose_terminal && logOuputLevel >= debugOut) {
        cout << TERMINAL_COLOR_DEBUG << timestamp() << msg << TERMINAL_COLOR_WHITE << endl;
	}
}

void Log::info(const std::string &msg) {
	rotating_logger->info(msg);
	if (verbose_terminal && logOuputLevel >= infoOut) {
        cout << TERMINAL_COLOR_INFO << timestamp() << msg << TERMINAL_COLOR_WHITE << endl;
    }
}

void Log::warn(const std::string &msg) {
	rotating_logger->warn(msg);
	if (verbose_terminal && logOuputLevel >= warnOut) {
        cout << TERMINAL_COLOR_WARN << timestamp() << msg << TERMINAL_COLOR_WHITE << endl;
    }
}

void Log::error(const std::string &msg) {
	rotating_logger->error(msg);
	if (verbose_terminal) cerr << TERMINAL_COLOR_ERROR << timestamp() << msg << TERMINAL_COLOR_WHITE << endl;
}

void Log::error(std::shared_ptr<queries::QueryStatus> &queryStatus,const std::string &msg) {
	error(msg);
	queryStatus->status = queries::HTTP_500;
	queryStatus->customMessage = msg;
}

void Log::panic(const std::string &msg) {
	rotating_logger->error(msg);
    cerr << TERMINAL_COLOR_FATAL << "PANIC!!!!!  " << msg << TERMINAL_COLOR_WHITE << endl;
	exit(0);
}
