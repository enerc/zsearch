#include "server.hpp"
#include "../../common/config.hpp"
#include "../../common/log.hpp"
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netdb.h>
#include "../../sql/SQLParser.hpp"
#include "../../queries/builders/ast.hpp"
#include "../../mapping/mappingParser.hpp"

using namespace std;
using namespace queries;
using namespace queries::builders;
using namespace zsql;
using namespace mapping;

namespace queries::executor {

constexpr const char* CMD_CREATE_INDEX = "/create_index";

static struct MHD_Daemon *userDaemon;
static struct MHD_Daemon *adminDaemon;
static std::shared_ptr<QueryStatus> qs = std::make_shared<QueryStatus>();
static std::mutex userMu;


void Server::start() {
    uint16_t userPort = Config::getInstance()->getUserPort();
    string ip = Config::getInstance()->getBindIp();
    sockaddr_in loopback_addr{};
    memset(&loopback_addr, 0, sizeof(loopback_addr));
    loopback_addr.sin_family = AF_INET;
    loopback_addr.sin_port = htons(userPort);
    struct hostent *hent;
    struct in_addr **addr_list;
    if ( (hent = gethostbyname( ip.c_str() ) ) == nullptr) {
        Log::panic("Invalid hostname " + ip);
    }
    addr_list = (struct in_addr **) hent->h_addr_list;
    if (addr_list[0] != nullptr) {
        loopback_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*addr_list[0]));
    } else {
        Log::panic("Failed to resolve IP for "+ip);
    }
    userDaemon = MHD_start_daemon (MHD_USE_THREAD_PER_CONNECTION | MHD_USE_INTERNAL_POLLING_THREAD , userPort, nullptr, nullptr,
                                   reinterpret_cast<MHD_AccessHandlerCallback>(&answer_to_user_connection), nullptr,
                                   MHD_OPTION_SOCK_ADDR, (struct sockaddr *)(&loopback_addr), MHD_OPTION_END);

    if (userDaemon == nullptr) {
        Log::panic("HTTP server failed to start. Check configuration (port/IP). Port may be already in use.");
    }
    Log::info("HTTP server started on port "+to_string(userPort) + " for user queries");

    uint16_t adminPort = Config::getInstance()->getAdminPort();
    if (adminPort == 0) {
        adminDaemon = nullptr;
        Log::warn("HTTP server on same port for users and admin. End users are allowed to truncate an index!!!. You are warned.");
    } else {
        loopback_addr.sin_port = htons(adminPort);
        adminDaemon = MHD_start_daemon (MHD_USE_THREAD_PER_CONNECTION | MHD_USE_INTERNAL_POLLING_THREAD, (uint16_t)userPort, nullptr, nullptr,
                                        &answer_to_admin_connection, nullptr, MHD_OPTION_SOCK_ADDR, (struct sockaddr *)(&loopback_addr), MHD_OPTION_END);

        if (adminDaemon == nullptr) {
            Log::panic("HTTP server failed to start. Check configuration (port/IP).");
        }
        Log::info("HTTP server started on port "+to_string(adminPort) + " for admins");
    }
}

void Server::stop() {
    MHD_stop_daemon(userDaemon);
    if (adminDaemon !=  nullptr) {
        MHD_stop_daemon(adminDaemon);
    }
}

static ssize_t file_reader (void *cls, uint64_t pos, char *buf, size_t max) {
    auto r = (MHDResponseHelper*)cls;
    if (r->currentFileFd == 0) {
        string f = r->filePath+to_string(r->currentFile) + ".res";
        r->currentFileFd = open(f.c_str(),O_RDONLY);
        if (r->currentFileFd < 0) {
            Log::error("Error in retrieving response file "+f);
            return MHD_CONTENT_READER_END_WITH_ERROR;
        }
    }
    int i;
    auto y = read(r->currentFileFd,buf,max);
    for (i=0; i< y; i++) {
        if (buf[i] == 0) {
            y = i;
            break;
        }
    }
    if (y < max) {
        close(r->currentFileFd);
        string f = r->filePath+to_string(r->currentFile) + ".res";
        remove(f.c_str());
        r->currentFile++;
        r->currentFileFd = 0;
    }
    return y;
}

static void file_free_callback (void *cls)  {
    auto r = (MHDResponseHelper*)cls;
    if (r->currentFileFd != 0) {
        close(r->currentFileFd);
        r->currentFileFd = 0;
    }
    // some might have already be removed
    for (int i=0; i< r->nbFiles; i++) {
        string f = r->filePath + to_string(i) + ".res";
        remove(f.c_str());
    }
    free(r);
}

MHD_Result Server::processQuery(struct MHD_Connection *connection,struct MHD_Response **response,const char *q,bool isAdmin) {
    MHD_Result ret;
    auto o = MHD_get_connection_info(connection,MHD_CONNECTION_INFO_CLIENT_ADDRESS);

    SQLParserResult result;
    SQLParser::parse(q,&result) ;
    string myIp = inet_ntoa(((sockaddr_in*)o->client_addr)->sin_addr);
    Log::info(string(q)+" ["+myIp+"]");
    std::shared_ptr<QueryStatus> queryStatus = make_shared<QueryStatus>();
    bool ok = result.isValid();
    int nbFiles;
    uint64_t resultSize;
    string i;
    if (ok) {
        AST ast(result.getStatement(0),queryStatus,q);
        Log::debug(result.asString());
        ok &= queryStatus->status == HTTP_200;
        if (ok) {
            // one query at a time
            userMu.lock();
            ast.iterate(isAdmin);
            userMu.unlock();
            if (queryStatus->status != HTTP_200) {
                *response = MHD_create_response_from_buffer(queryStatus->customMessage.size(), (void *) queryStatus->customMessage.c_str(), MHD_RESPMEM_MUST_COPY);
                if (queryStatus->status == HTTP_204) {
                    ret = MHD_queue_response (connection, HTTP_200, *response);
                } else {
                    ret = MHD_queue_response (connection, queryStatus->status, *response);
                }
            } else {
                i = ast.getResultAsFiles(nbFiles, resultSize);
                if (queryStatus->status == HTTP_204) {
                    *response = MHD_create_response_from_buffer(queryStatus->customMessage.size(), (void *) queryStatus->customMessage.c_str(), MHD_RESPMEM_MUST_COPY);
                    ret = MHD_queue_response (connection, MHD_HTTP_OK, *response);
                } else {
                    auto r = (MHDResponseHelper *) malloc(sizeof(MHDResponseHelper));
                    r->currentFile = 0;
                    r->currentFileFd = 0;
                    r->nbFiles = nbFiles;
                    memcpy(r->filePath, i.c_str(), i.size());
                    r->filePath[i.size()] = 0;
                    *response = MHD_create_response_from_callback(resultSize, 1024 * 1024, &file_reader, r, &file_free_callback);
                    if (*response == nullptr) {
                        return MHD_NO;
                    }
                    ret = MHD_queue_response (connection, MHD_HTTP_OK, *response);
                }
            }
        } else {
            *response = MHD_create_response_from_buffer(queryStatus->customMessage.size(), (void *) queryStatus->customMessage.c_str(), MHD_RESPMEM_MUST_COPY);
            ret = MHD_queue_response (connection, queryStatus->status, *response);
        }
    } else {
        auto err = string("Given string is not a valid SQL query. ") + result.errorMsg() + "      (L:" + to_string(result.errorLine()) + ":" + to_string(result.errorColumn()) + ")";
        Log::error(err);
        *response = MHD_create_response_from_buffer(err.size(), (void *) err.c_str(), MHD_RESPMEM_MUST_COPY);
        ret = MHD_queue_response (connection, MHD_HTTP_BAD_REQUEST, *response);
    }
    return ret;
}

void Server::processQueryCreateTable(struct MHD_Connection *connection,struct MHD_Response **response,const char *q,bool isAdmin) {
    SQLParserResult result;
    SQLParser::parse(q,&result) ;
    auto o = MHD_get_connection_info(connection,MHD_CONNECTION_INFO_CLIENT_ADDRESS);
    string myIp = inet_ntoa(((sockaddr_in*)o->client_addr)->sin_addr);
    Log::debug("Create index with "+string(q)+" ["+myIp+"]");

    const char *indexName = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "name");
    if (indexName == nullptr) {
        qs->customMessage = "Correct syntax is /create_index?name=myIndex and JSON as data";
        qs->status = HTTP_500;
    } else {
        qs->customMessage={};
        qs->status = HTTP_200;
        shared_ptr<IndexDefinition> def = IndexDefinitionManager::getInstance()->getIndex(indexName);
        if (def == nullptr) {
            def = IndexDefinitionManager::getInstance()->createIndex(indexName);
        }
        MappingParser p(def,q,qs);
        p.parse();
    }
}

MHD_Result Server::answer_to_user_connection(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data,
                                 size_t *upload_data_size, void **con_cls) {
    static int aptr;
    if (adminDaemon != nullptr ) {
        if (0 != strcmp(method, MHD_HTTP_METHOD_GET)) {
            return MHD_NO;
        }
    } else {
        if (0 != strcmp(method, MHD_HTTP_METHOD_GET) && 0 != strcmp(method, MHD_HTTP_METHOD_POST)) {
            return MHD_NO;
        }
    }
    if (&aptr != *con_cls)  {
        // do never respond on first call
        *con_cls = &aptr;
        qs->customMessage={};
        qs->status = HTTP_200;
        return MHD_YES;
    }
    *con_cls = nullptr;                  // reset when done

    struct MHD_Response *response;
    MHD_Result ret;
    const char *q = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "query");

    if (q != nullptr) {
        ret = processQuery(connection,&response,q,adminDaemon == nullptr);
    } else {
        if (adminDaemon != nullptr ) {
            static string s = "No query found in http user request. Use query=select A from Foo where ....";
            response = MHD_create_response_from_buffer(s.size(), (void *) s.c_str(), MHD_RESPMEM_MUST_COPY);
            ret = MHD_queue_response (connection, MHD_HTTP_BAD_REQUEST, response);
        } else {
            if (string(url) == string(CMD_CREATE_INDEX)) {
                if (*upload_data_size > 0) {
                    string s(upload_data,*upload_data_size);
                    processQueryCreateTable(connection,&response,s.c_str(),true);
                    *upload_data_size = 0;
                    return MHD_YES;
                }
                response = MHD_create_response_from_buffer(qs->customMessage.size(), (void *) qs->customMessage.c_str(), MHD_RESPMEM_MUST_COPY);
                ret = MHD_queue_response (connection, qs->status, response);
            } else {
                static string s = "No valid action found in http admin request.";
                response = MHD_create_response_from_buffer(s.size(), (void *) s.c_str(), MHD_RESPMEM_MUST_COPY);
                ret = MHD_queue_response (connection, MHD_HTTP_BAD_REQUEST, response);
            }
        }
    }
    MHD_destroy_response (response);
    return ret;
}

MHD_Result Server::answer_to_admin_connection(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data,
                                      size_t *upload_data_size, void **con_cls) {
    static uint64_t aptr;
    if (0 != strcmp(method, MHD_HTTP_METHOD_GET) && 0 != strcmp(method, MHD_HTTP_METHOD_POST)) {
        return MHD_NO;
    }
    if (&aptr != *con_cls)  {
        // do never respond on first call
        *con_cls = &aptr;
        aptr = 0x0;
        return MHD_YES;
    }
    *con_cls = nullptr;                  // reset when done

    struct MHD_Response *response;
    MHD_Result ret;
    const char *q0 = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "query");

    if (q0 != nullptr) {
        ret = processQuery(connection,&response,q0,true);
    } else {
        if (string(url) == string(CMD_CREATE_INDEX)) {
            if (*upload_data_size > 0) {
                string s(upload_data,*upload_data_size);
                processQueryCreateTable(connection,&response,s.c_str(),true);
                *upload_data_size = 0;
                return MHD_YES;
            }
            response = MHD_create_response_from_buffer(qs->customMessage.size(), (void *) qs->customMessage.c_str(), MHD_RESPMEM_MUST_COPY);
            ret = MHD_queue_response (connection, qs->status, response);
        } else {
            static string s = "No valid action found in http admin request.";
            response = MHD_create_response_from_buffer(s.size(), (void *) s.c_str(), MHD_RESPMEM_MUST_COPY);
            ret = MHD_queue_response (connection, MHD_HTTP_BAD_REQUEST, response);
        }
    }
    MHD_destroy_response (response);
    return ret;
}

}