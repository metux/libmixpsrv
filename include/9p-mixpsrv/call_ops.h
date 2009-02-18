
#ifndef __IXPSERV_CALL_OPS_H
#define __IXPSERV_CALL_OPS_H

#include <9p-mixp/mixp.h>
#include <9p-mixpsrv/types.h>
#include <9p-mixpsrv/default_ops.h>

/* -- default operations - will also be used internally -- */
static inline int mixpsrv_call_ops_open(MIXPSRV_FILE_HANDLE* f, long mode)
{
    if (f->ops.open == NULL)
	return mixpsrv_default_ops_open(f, mode);
    else
	return f->ops.open(f, mode);
}

static inline long mixpsrv_call_ops_read(MIXPSRV_FILE_HANDLE* f, long offset, size_t size, void* buf)
{
    if (f->ops.read == NULL)
	return mixpsrv_default_ops_read(f, offset, size, buf);
    else
	return f->ops.read(f, offset, size, buf);
}

static inline long mixpsrv_call_ops_write(MIXPSRV_FILE_HANDLE* f, long offset, size_t size, void* buf)
{
    if (f->ops.write == NULL)
	return mixpsrv_default_ops_write(f, offset, size, buf);
    else
	return f->ops.write(f, offset, size, buf);
}

static inline long mixpsrv_call_ops_size(MIXPSRV_FILE_HANDLE* f)
{
    if (f->ops.size == NULL)
	return mixpsrv_default_ops_size(f);
    else
	return f->ops.size(f);
}

static inline int mixpsrv_call_ops_stat(MIXPSRV_FILE_HANDLE* f, MIXP_STAT* s)
{
    if (f->ops.stat == NULL)
	return mixpsrv_default_ops_stat(f, s);
    else
	return f->ops.stat(f, s);
}

static inline int mixpsrv_call_ops_close(MIXPSRV_FILE_HANDLE* f)
{
    if (f->ops.close == NULL)
	return mixpsrv_default_ops_close(f);
    else
	return f->ops.close(f);
}

#endif
