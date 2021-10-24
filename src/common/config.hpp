#pragma once

#include <string>

class Config {
public:
	static Config* getInstance();
	static void init();

	[[nodiscard]] std::string getLogPath() const {
		return logPath;
	}

    [[maybe_unused]] void setLogPath(const std::string &_logPath) {
		logPath = _logPath;
	}

	[[nodiscard]] size_t getMaxLogFileSize() const {
		return maxLogFileSize;
	}

    [[maybe_unused]] void setMaxLogFileSize(size_t _maxLogFileSize) {
		maxLogFileSize = _maxLogFileSize;
	}

	[[nodiscard]] uint32_t getMaxRotatedFiles() const {
		return maxRotatedFiles;
	}

    [[maybe_unused]] void setMaxRotatedFiles(uint32_t _maxRotatedFiles) {
		maxRotatedFiles = _maxRotatedFiles;
	}

	[[nodiscard]] std::string getLogLevel() const {
		return logLevel;
	}

    [[maybe_unused]] void setLogLevel(const std::string &_logLevel) {
		logLevel = _logLevel;
	}

	[[nodiscard]] std::string getDictionaryPath() const {
		return dictionaryPath;
	}

    [[maybe_unused]] void setDictionaryPath(const std::string &_dictionaryPath) {
		dictionaryPath = _dictionaryPath;
	}

	[[nodiscard]] std::string getIndexPath() const {
		return indexPath;
	}

    [[maybe_unused]] void setIndexPath(const std::string &_indexPath) {
		indexPath = _indexPath;
	}

	[[nodiscard]] std::string getResultTempPath() const {
		return resultTempPath;
	}

    [[maybe_unused]] void setResultTempPath(const std::string &_resultTempPath) {
        resultTempPath = _resultTempPath;
	}

    [[nodiscard]] uint32_t getUserPort() const {
        return userPort;
    }

    [[nodiscard]] uint32_t getAdminPort() const {
        return adminPort;
    }

    [[nodiscard]] const std::string &getBindIp() const {
        return bindIP;
    }

    [[nodiscard]] uint64_t getLimit() const { return limit; }
private:
	static Config *instance;

	// logs
	std::string logPath;
	size_t		maxLogFileSize;
	uint32_t	maxRotatedFiles;
	std::string	logLevel;

	// dictionary
	std::string	dictionaryPath;

	// chunks
	std::string	indexPath;

	//  query result temp path
	std::string resultTempPath;

	// http
	uint32_t    userPort;       // select only
	uint32_t    adminPort;      // insert/select/delete/manage index definition
    std::string bindIP;         // 127.0.0.1 or 0.0.0.0 or local IP

    // limit
    uint64_t limit;

    // compiler warning
    int         err;
	Config();
	void        readConfigFile();
};
