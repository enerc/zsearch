#pragma once

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>
#include <string>
#include <memory>
#include "../../queries/queryStatus.hpp"

#if MHD_VERSION < 0x00097313
typedef int MHD_Result;
#endif

namespace queries::executor {

typedef struct {
    int currentFileFd;
    int currentFile;
    int nbFiles;
    char filePath[256];
} MHDResponseHelper;

class Server {
public:
    static void start();
    static void stop();
private:
    static MHD_Result answer_to_user_connection (void *cls, struct MHD_Connection *connection,const char *url,const char *method, const char *version,const char *upload_data,size_t *upload_data_size, void **con_cls);
    static MHD_Result answer_to_admin_connection (void *cls, struct MHD_Connection *connection,const char *url,const char *method, const char *version,const char *upload_data,size_t *upload_data_size, void **con_cls);
    static MHD_Result processQuery(struct MHD_Connection *connection,struct MHD_Response **response,const char *q,bool isAdmin);
    static void processQueryCreateTable(struct MHD_Connection *connection,struct MHD_Response **response,const char *q,bool isAdmin);

};

}