
#ifndef __MIXP_SERVER_H
#define __MIXP_SERVER_H

#include <9p-mixpsrv/types.h>

const char* mixp_9pserver_run_tcp(
    const char* hostname,
    int port,
    MIXPSRV_FILE_HANDLE*(*openroot)(),
    void* priv
);

#endif
