#include <csignal>
#include <getopt.h>
#include "common/log.hpp"
#include "common/config.hpp"
#include "../git.h"
#include "../config.h"
#include "queries/executor/server.hpp"

#define TERMINAL_COLOR_WELCOME "\033[38;5;208m"
extern void createTestFiles();
extern void checkKernels(bool _doBench);

using namespace std;

void signalHandler( int signum ) {
    queries::executor::Server::stop();
    exit(signum);
}

void printVersion() {
    if (verbose_terminal) {
        cout << TERMINAL_COLOR_WELCOME;

        cout << " _____"<<endl;
        cout << " \\__  )                            | |"<<endl;
        cout << "   / /      ___  ___  __ _ _ __ ___| |__"<<endl;
        cout << "  / /      / __|/ _ \\/ _` | '__/ __| '_ \\" << endl;
        cout << " | |__  -  \\__ \\  __/ (_| | | | (__| | | |"<<endl;
        cout << "  \\__ \\    |___/\\___|\\__,_|_|  \\___|_| |_|       " <<  "V"<<VERSION<<"-\e[3m" << string(GIT_HEAD_SHA1).substr(0,9) <<endl;
        cout << "     ) )"<<endl;
        cout << "    (_/"<<endl;
        cout << "\033[0m" << endl << endl;
    }
    Log::info(string("ζsearch V") + VERSION+" build:" + string(GIT_HEAD_SHA1).substr(0,9) + +" started");
}

void printHelp()  {
    cout << "./zsearch [--verbose] [--help] [--contact]" << endl << endl;
    cout << "--verbose    Display the logs in the terminal console" << endl;
    cout << "--contact    Display professional services contact information" << endl;
    cout << "--check      Check kernel. Use --check=y if you want tpo run benchmaks of the kernels" << endl;
    exit(0);
}

void printContact() {
    uint64_t key = 0x456d48de026bc47aL;
    constexpr uint64_t CTX[]={0x2c0c3aaa2219ab3cL,0x370c68f26505ad14L,0x30192bbb7602ac19L,0x230327bd2247a108L,0x2a043cbf701ea313L,0x33082cfe7004e414L,0x31032db37204a81fL,0x651a2db0220dab5aL,0x36083aab760aa11cL,0x201e29bb6e1be456L,0x310e29aa6c04a75aL,0x26432bac6705a15aL,0x2b043cb27718aa15L,0x6b0121bf6f0c841dL,0x244d01d42c06ab19L,0x20083ab8220ae417L,0x370c68bb6105a516L,0x65192bbb7602ac19L,0x2b0c25fe6a1fad0dL,0x651e3abf6712e403L,0x370838a6674ba215L,0x2b0268bb6105a113L,0x240168a7700eb25aL,0x2f023aae220ea308L,0x654d68f0711fa71fL,0x456d48fe224be45aL};
    cout << endl;
    string s;
    for (const auto &o : CTX) {
        uint64_t a = o ^ key;
        s += string((const char*)&a,8);
    }
    cout << s << endl << endl;
    exit(0);
}

void processArgs(int argc, char **argv) {
    const char *const short_opts = ":v:c:h";
    const option long_opts[] = {
            {"verbose",   no_argument, nullptr, 'v'},
            {"contact",   no_argument, nullptr, 'c'},
            {"check",     optional_argument, nullptr, 'k'},
            {"help",      no_argument, nullptr, 'h'},
            {nullptr,     no_argument, nullptr, 0}
    };

    while (true) {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

        if (-1 == opt)
            break;

        switch (opt) {
            case 'v':
                verbose_terminal = true;
                std::cout << "verbose_terminal " << std::endl;
                break;
            case 'c':
                printContact();
                break;
            case 'k': {
                bool b = optarg != nullptr;
                checkKernels(b);
                cout << endl << "Download taxi.json at https://rally-tracks.elastic.co/nyc_taxis/documents.json.bz2"<<endl;
                cout << "Uncompress this file and compress it again with lz4 and name it taxis.json.lz4" << endl;
                cout << "Put or link this files under bin/tests" << endl;
                cout << "Copy tests/queries.py to bin/tests"<<endl;
                cout << "In bin/tests run 'python3 queries.py' and check the results" << endl << endl;
                createTestFiles();
                exit(0);
                }
                break;
            case 'h':
            case '?': // Unrecognized option
            default:
                printHelp();
                break;
        }
    }
}

int main(int argc, char *argv[]) {
    Config::init();
    Log::init();
    processArgs(argc,argv);
    printVersion();
    //createTestFiles();
    //exit(0);
    //repository::ImportFile ifile("tests/taxis.json.lz4","nyc_taxis");
    //std::shared_ptr<queries::QueryStatus> qs = std::make_shared<queries::QueryStatus>();
    ///return ifile.doImport(qs);
    //auto i = indexes::IndexRepository::getInstance()->getIndex("nyc_taxis","total_amount");
    //i->showChunkStates("total_amount");
    //exit(0);

    //checkKernels(false); exit(0);
    queries::executor::Server::start();
    signal(SIGINT, signalHandler);

    //cout << dateFTToSeconds("1970-12-14 16:41:23") << endl;

    //initCommonEnglishWordTest();
    //createMySqlTaxisFile();
    //testImportNewYorkTaxiDatabase();
    //testSqlQueryEnglishWord();
    //testSqlQueryTaxis();
    //joinTest();
    //shared_ptr<IndexManager> indexManager = IndexRepository::getInstance()->getIndex("nyc_taxis","total_amount");
    //shared_ptr<IndexChunk> c = indexManager->getChunkForRead(0);
   while(true) {
       sleep(1000);
   }
   return 0;
}
