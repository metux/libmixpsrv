
#ifndef __IXPSERV_DEFAULT_OPS_H
#define __IXPSERV_DEFAULT_OPS_H

#include <9p-mixp/mixp.h>
#include <9p-mixpsrv/types.h>

/* -- default operations - will also be used internally -- */
int  mixpsrv_default_ops_open(MIXPSRV_FILE_HANDLE* f, long mode);
long mixpsrv_default_ops_read(MIXPSRV_FILE_HANDLE* f, long offset, size_t size, void* buf);
long mixpsrv_default_ops_write(MIXPSRV_FILE_HANDLE* f, long offset, size_t size, void* buf);
long mixpsrv_default_ops_size(MIXPSRV_FILE_HANDLE* f);
int  mixpsrv_default_ops_stat(MIXPSRV_FILE_HANDLE* f, MIXP_STAT* s);

extern MIXPSRV_FILE_OPS mixp_default_fileops;

#endif
