
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <9p-mixp/mixp.h>
#include <9p-mixpsrv/types.h>
#include <9p-mixpsrv/default_ops.h>
#include <9p-mixpsrv/call_ops.h>

/* Macros */
#define QID(t, i) (((long long)((t)&0xFF)<<32)|((i)&0xFFFFFFFF))

int mixpsrv_default_ops_open(MIXPSRV_FILE_HANDLE* f, long mode)
{
	/* if we don't have an own open() function, this is the default */
	if((mode&3) == P9_OEXEC) 
	{
		printf("1\n");
		return -EPERM;
	}
	if((mode&3) != P9_OREAD && !(f->perm & 0200)) {
		printf("2\n");
		return -EPERM;
	}
//	if((mode&3) != P9_OWRITE && !(f->tab.perm & 0400)) {
//		printf("3\n");
//		return -EPERM;
//	}
	if((mode&~(3|P9_OAPPEND|P9_OTRUNC))) {
		printf("4\n");
		return -EPERM;
	}
	return 0;
}

long mixpsrv_default_ops_read(MIXPSRV_FILE_HANDLE* f, long offset, size_t size, void* buffer)
{
    printf("mixp_defaultops_read() no data.\n");
    return 0;
}

long mixpsrv_default_ops_write(MIXPSRV_FILE_HANDLE* f, long offset, size_t size, void* buffer)
{
    fprintf(stderr,"mixpsrv_default_ops_write() offset=%ld size=%uld\n", offset, size);
    return -EPERM;
}

long mixpsrv_default_ops_size(MIXPSRV_FILE_HANDLE* f)
{
#ifdef _DEBUG
    fprintf(stderr,"mixpsrv_default_ops_size(): zero .. name=\"%s\"\n", f->name);
#endif
    return 0;
}

int mixpsrv_default_ops_close(MIXPSRV_FILE_HANDLE* f)
{
    fprintf(stderr,"mixpsrv_default_ops_close(): close unsupported\n");
    return 0;
}

int mixpsrv_default_ops_stat(MIXPSRV_FILE_HANDLE* f, MIXP_STAT *s) 
{
    s->type        = 0;
    s->dev         = 0;
    s->qid.path    = QID(f->type, f->id);
    s->qid.version = 0;
    s->qid.type    = f->qtype;
    s->mode        = f->perm;
    s->atime       = time(NULL);
    s->mtime       = time(NULL);
    s->name        = (f->name ? f->name : "???");
    s->uid 	   = s->gid = s->muid = "INCOGNITO";
    s->length      = mixpsrv_call_ops_size(f);

    return 1;
}

MIXPSRV_FILE_OPS mixp_default_fileops = {
    .open      = mixpsrv_default_ops_open,
    .read      = mixpsrv_default_ops_read,
    .write     = mixpsrv_default_ops_write,
    .size      = mixpsrv_default_ops_size,
    .stat      = mixpsrv_default_ops_stat,
    .classname = "default"
};
