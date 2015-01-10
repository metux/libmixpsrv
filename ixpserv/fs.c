
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>

#include <9p-mixp/mixp.h>
#include <9p-mixp/fcall.h>
#include <9p-mixpsrv/types.h>
#include <9p-mixpsrv/p9srv.h>
#include <9p-mixpsrv/prototypes.h>
#include <9p-mixpsrv/errors.h>

extern MIXPSRV_FILE_HANDLE* open_root();

MIXPSRV_FILESERVER fileserver = {
	.magic = MIXPSRV_FILESERVER_MAGIC,
	.openRoot = open_root
};

/* --------------------- root directory ops ----------------------- */

MIXPSRV_FILE_HANDLE* rootops_lookup(MIXPSRV_FILE_HANDLE*parent, const char* name)
{
    printf("rootops_lookup() requested: \"%s\"\n",name);

    if ((!name) || (!strlen(name)) || (!strcmp(name,"/")))
    {
	printf("rootops_lookup() requested the root directory\n");
	return open_root();
    }

    if (!strcmp(name,"etc"))
    {
	printf(" --> /etc\n");
	return p9srv_get_file();
    }
    
    if (!strcmp(name,"foo"))
    {
	printf(" --> /foo\n");
	MIXPSRV_FILE_HANDLE* n = p9srv_get_file();
	mixpsrv_hostfile_ops_init(n,"/etc/hosts");
	return n;
    }

    return NULL;
}

MIXPSRV_FILE_OPS rootops = {
    .size      = NULL,
    .open      = NULL,
    .lookup    = rootops_lookup,
    .classname = "etcdir"
};

MIXPSRV_FILE_HANDLE* open_root(MIXPSRV_FILESERVER* server)
{
	MIXPSRV_FILE_HANDLE *f = p9srv_get_file();
	f->ops   = rootops;
	f->name  = strdup("/");
	f->qtype = P9_QTDIR;
	f->type  = 0;
	f->perm  = 0500|P9_DMDIR;
	return f;
}

int main()
{
    printf("starting 9p \n");
    char* errstr = "OKAY";
    int fd;
    
    fd = ixp_serversock_tcp("*",9999,&errstr);
    if (fd<0)
    {
	printf("fd=%d err=%s\n",fd,errstr);
	return -1;
    }

    p9srv_run_server(fd, &fileserver);
    return 0;
}
