
#ifndef __MIXPSRV_DEFAULT_OPS_H
#define __MIXPSRV_DEFAULT_OPS_H

#include <9p-mixp/mixp.h>
#include <9p-mixpsrv/types.h>

/* -- default operations - will also be used internally -- */
int  mixpsrv_default_ops_open(MIXPSRV_FILE_HANDLE* f, long mode);
long mixpsrv_default_ops_read(MIXPSRV_FILE_HANDLE* f, long offset, size_t size, void* buf);
long mixpsrv_default_ops_write(MIXPSRV_FILE_HANDLE* f, long offset, size_t size, void* buf);
long mixpsrv_default_ops_size(MIXPSRV_FILE_HANDLE* f);
int  mixpsrv_default_ops_stat(MIXPSRV_FILE_HANDLE* f, MIXP_STAT* s);
int  mixpsrv_default_ops_close(MIXPSRV_FILE_HANDLE* f);

extern MIXPSRV_FILE_OPS mixp_default_fileops;

/* some helper macros for the lookup() ops */

#define MIXPSRV_LOOKUP_BEGIN						\
    MIXPSRV_FILE_HANDLE* ___lookup_first = NULL;			\
    MIXPSRV_FILE_HANDLE* ___lookup_walk  = NULL;			\

#define MIXPSRV_LOOKUP_ADD(par)					\
    {									\
	MIXPSRV_FILE_HANDLE* ___lookup_tmp = par;			\
	if (par)							\
	{								\
	    if (___lookup_first == NULL)				\
		___lookup_first = ___lookup_walk = ___lookup_tmp;	\
	    else							\
	    {								\
		___lookup_walk->next = ___lookup_tmp;			\
		___lookup_walk = ___lookup_tmp;				\
	    }								\
	}								\
    }

#define MIXPSRV_LOOKUP_END return ___lookup_first;

#endif
