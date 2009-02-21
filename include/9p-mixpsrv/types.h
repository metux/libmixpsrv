
#ifndef __IXP_TYPES_H
#define __IXP_TYPES_H

#include <9p-mixp/mixp.h>

typedef struct _MIXPSRV_FILE_OPS    MIXPSRV_FILE_OPS;
typedef struct MIXPSRV_FILE_HANDLE  MIXPSRV_FILE_HANDLE;
typedef struct MIXPSRV_FILESERVER   MIXPSRV_FILESERVER;
typedef struct _MIXPSRV_IORESULT    MIXPSRV_IORESULT;

#define MIXPSRV_FILESERVER_MAGIC 	123321

struct _MIXPSRV_IORESULT {
	int    err;
	size_t size;
};

struct MIXPSRV_FILESERVER {
	int magic;
	MIXPSRV_FILE_HANDLE* (*openRoot)(MIXPSRV_FILESERVER*);
	int refcount;
	struct
	{
	    char* text;
	    void* ptr;
	    int   status;
	} priv;
};

struct _MIXPSRV_FILE_OPS {
	const char*  classname;
	int          (*open)(MIXPSRV_FILE_HANDLE* f, long mode);
	MIXPSRV_FILE_HANDLE*  (*lookup)(MIXPSRV_FILE_HANDLE* parent, const char *name);
	long         (*size)(MIXPSRV_FILE_HANDLE* f);
	int          (*unlink)(MIXPSRV_FILE_HANDLE* f);
	long         (*read)(MIXPSRV_FILE_HANDLE* f, long offset, size_t size, void* buffer);
	long         (*write)(MIXPSRV_FILE_HANDLE* f, long offset, size_t size, void* buffer);
        int          (*stat)(MIXPSRV_FILE_HANDLE* f, MIXP_STAT* stat);
	int          (*close)(MIXPSRV_FILE_HANDLE* f);
};

struct MIXPSRV_FILE_HANDLE {
	MIXPSRV_FILE_HANDLE		*next;
	unsigned int	id;
	unsigned short	nref;
	char* 		name;
	unsigned char   qtype;
	unsigned int    type;

	int		perm;
	MIXPSRV_FILE_OPS	ops;
	
	// private fields - reserved for the ops handlers
	// you can freely use them in you own ops functions
	// libmixpserv won't touch them
	struct
	{
		int 	    fd;
		char* 	    text;
		const char* name;
		void*       ptr;
	} priv;
};

#endif
