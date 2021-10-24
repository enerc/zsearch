#include "config.hpp"
#include "tools.hpp"
#include <filesystem>
#include <iostream>
#include <cstdio>

using namespace std;

Config* Config::instance = nullptr;

Config* Config::getInstance() {
	if (instance == nullptr)  {
		instance = new Config();
	}
	return instance;
}

void Config::init() {
    getInstance();
}

Config::Config() {
    logPath = "./error.log";
    logLevel = "DEBUG";
    maxLogFileSize = 100L * 1024L * 1024L;    // default to 100 Mb
    maxRotatedFiles = 10;
    dictionaryPath = "./dictionary";
    indexPath = "./indexes";
    resultTempPath = "./tmp";
    userPort = 9090;
    adminPort = 9091;
    bindIP = "127.0.0.1";
    limit = 1000;

    readConfigFile();

    // create dictionary folder if it does not exists
    if (!filesystem::is_directory(dictionaryPath) || !filesystem::exists(dictionaryPath)) { // Check if src folder exists
        filesystem::create_directory(dictionaryPath);
    }

    // create index folder if it does not exists
    if (!filesystem::is_directory(indexPath) || !filesystem::exists(indexPath)) { // Check if src folder exists
        filesystem::create_directory(indexPath);
    }

    // recreate the Temp path. Make sure it's empty
    if (!filesystem::is_directory(resultTempPath) || !filesystem::exists(resultTempPath)) { // Check if src folder exists
        filesystem::create_directory(resultTempPath);
    }
    for (const auto &dirEntry : std::filesystem::directory_iterator(resultTempPath)) {
        if (endsWith(dirEntry.path(),".res")) {
            string cmd = string("rm  ") + dirEntry.path().string();
            cout << cmd << endl;
            err = system(cmd.c_str());
        }
    }
}

void Config::readConfigFile() {
    char * line = nullptr;
    size_t len = 0;
    ssize_t read;
    FILE *f = fopen("config.txt","r");
    if (f == nullptr) {
        cout << "No configuration file found (config.txt) - Using default settings " << endl << endl;
        return;
    }

    while ((read = getline(&line, &len, f)) != -1) {
        // skip comments
        if (read > 2 && line[0]=='/' && line[1]=='/') {
            continue;
        }
        string k,v;
        int i=0;
        while (i < read-1 && line[i] == ' ') i++;
        while (i < read-1 && line[i] != ' ') k += line[i++];
        while (i < read-1 && line[i] == ' ') i++;
        while (i < read-1 && line[i] == '=') i++;
        while (i < read-1 && line[i] == ' ') i++;
        while (i < read-1 && line[i] != ' ') v += line[i++];

        if (k == "LogPath") logPath = v;
        if (k == "LogLevel") logLevel = v;
        if (k == "MaxLogFileSize") maxLogFileSize = stoi(v)*1024*1024;
        if (k == "MaxRotatedFiles") maxRotatedFiles = stoi(v);
        if (k == "DictionaryPath") dictionaryPath = v;
        if (k == "IndexPath") indexPath = v;
        if (k == "ResultTempPath") resultTempPath = v;
        if (k == "BindIP") bindIP = v;
        if (k == "UserPort") userPort = stoi(v);
        if (k == "AdminPort") adminPort = stoi(v);
        if (k == "Limit") limit = stoi(v);
        if (limit == 0) limit = UINT64_MAX;
    }

    fclose(f);
}


