
#ifndef __IXPSERV_P9CLIENT_OPS_H
#define __IXPSERV_P9CLIENT_OPS_H

#include <9p-mixp/mixp.h>
#include <9p-mixpsrv/types.h>

/* -- plain file access. represents one file per MIXPSRV_FILE_HANDLE in host's fs -- */
int  mixpsrv_p9client_ops_init(MIXPSRV_FILE_HANDLE* f, const char* filename);
int  mixpsrv_p9client_ops_open(MIXPSRV_FILE_HANDLE* f, long mode);
long mixpsrv_p9client_ops_read(MIXPSRV_FILE_HANDLE* f, long offset, size_t size, void* buf);
long mixpsrv_p9client_ops_size(MIXPSRV_FILE_HANDLE* f);

#endif
