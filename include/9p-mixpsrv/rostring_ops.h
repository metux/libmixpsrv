
#ifndef __IXPSERV_ROSTRING_OPS_H
#define __IXPSERV_ROSTRING_OPS_H

#include <9p-mixp/mixp.h>
#include <9p-mixpsrv/types.h>

/* -- file access to an in-memory string (read-only) -- */
int         mixpsrv_rostring_ops_init(MIXPSRV_FILE_HANDLE* f, const char* filename);
int         mixpsrv_rostring_ops_open(MIXPSRV_FILE_HANDLE* f, long mode);
long        mixpsrv_rostring_ops_read(MIXPSRV_FILE_HANDLE* f, long offset, size_t size, void* buf);
long        mixpsrv_rostring_ops_size(MIXPSRV_FILE_HANDLE* f);
int         mixpsrv_rostring_ops_close(MIXPSRV_FILE_HANDLE* f);
MIXPSRV_FILE_HANDLE* mixpsrv_rostring_ops_create(const char* str);

static MIXPSRV_FILE_OPS mixpsrv_rostring_ops = {
    .open 	= mixpsrv_rostring_ops_open,
    .read 	= mixpsrv_rostring_ops_read,
    .size 	= mixpsrv_rostring_ops_size,
    .close      = mixpsrv_rostring_ops_close,
    .classname 	= "rostring"
};

#endif
