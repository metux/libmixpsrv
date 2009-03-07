
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <9p-mixpsrv/types.h>
#include <9p-mixpsrv/p9srv.h>
#include <9p-mixpsrv/rostring_ops.h>

int mixpsrv_rostring_ops_open(MIXPSRV_FILE_HANDLE* f, long mode)
{
    // FIXME: we currently are only working RO on plain files.
    return 0;
}

long mixpsrv_rostring_ops_read(MIXPSRV_FILE_HANDLE* f, long offset, size_t size, void* buf)
{
    if (!f->priv.name)
	return 0;

    if (offset>=(f->priv.fd))
    {
	fprintf(stderr,"ixp_rostring_ops_read() attempt to ready beyond end of file\n");
	return 0;
    }
    else if ((offset+size)>(f->priv.fd))
    {
	size = (f->priv.fd) - offset;
	fprintf(stderr,"ixp_rostring_ops_read() reducing size to %uld @ offset %ld size %d\n", size, offset, f->priv.fd);
    }

    memcpy(buf,(f->priv.name)+offset, size);
    return size;
}

MIXPSRV_FILE_HANDLE* mixpsrv_rostring_ops_create(const char* str)
{
    MIXPSRV_FILE_HANDLE* f = p9srv_get_file();
    mixpsrv_rostring_ops_init(f,str);
    return f;
}

int mixpsrv_rostring_ops_init(MIXPSRV_FILE_HANDLE* f, const char* str)
{
    f->priv.name = (str ? strdup(str) : strdup(""));
    f->priv.fd  = strlen(f->priv.name);
    f->ops = mixpsrv_rostring_ops;
    return 0;
}

long mixpsrv_rostring_ops_size(MIXPSRV_FILE_HANDLE* f)
{
    return f->priv.fd;
}

int mixpsrv_rostring_ops_stat(MIXPSRV_FILE_HANDLE* f, MIXP_STAT* s)
{
    fprintf(stderr,"ixp_rostring_ops_size() stat not implemented yet\n");
    return 0;
}

int mixpsrv_rostring_ops_close(MIXPSRV_FILE_HANDLE* f)
{
    if (f->priv.name)
    {
	free((char*)f->priv.name);
	f->priv.name = NULL;
	f->priv.fd   = 0;
    }
    return 0;
}
