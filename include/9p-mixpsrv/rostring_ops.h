
#ifndef __IXPSERV_ROSTRING_OPS_H
#define __IXPSERV_ROSTRING_OPS_H

#include <9p-mixp/mixp.h>
#include <9p-mixpsrv/types.h>

/* -- file access to an in-memory string (read-only) -- */
int         mixp_rostring_ops_init(MIXPSRV_FILE_HANDLE* f, const char* filename);
int         mixp_rostring_ops_open(MIXPSRV_FILE_HANDLE* f, long mode);
long        mixp_rostring_ops_read(MIXPSRV_FILE_HANDLE* f, long offset, size_t size, void* buf);
long        mixp_rostring_ops_size(MIXPSRV_FILE_HANDLE* f);
MIXPSRV_FILE_HANDLE* mixp_rostring_ops_create(const char* str);

static MIXPSRV_FILE_OPS ixp_rostring_ops = {
    .open 	= mixp_rostring_ops_open,
    .read 	= mixp_rostring_ops_read,
    .size 	= mixp_rostring_ops_size,
    .classname 	= "rostring"
};

#endif
