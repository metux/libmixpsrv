
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include <9p-mixpsrv/types.h>
#include <9p-mixpsrv/p9srv.h>
#include <9p-mixpsrv/rostring_ops.h>

int mixp_rostring_ops_open(MIXPSRV_FILE_HANDLE* f, long mode)
{
    // FIXME: we currently are only working RO on plain files.
    return 0;
}

long mixp_rostring_ops_read(MIXPSRV_FILE_HANDLE* f, long offset, size_t size, void* buf)
{
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

MIXPSRV_FILE_HANDLE* mixp_rostring_ops_create(const char* str)
{
    MIXPSRV_FILE_HANDLE* f = p9srv_get_file();
    mixp_rostring_ops_init(f,str);
    return f;
}

int mixp_rostring_ops_init(MIXPSRV_FILE_HANDLE* f, const char* str)
{
    if (str==NULL)
	f->priv.name = "";
    else
	f->priv.name = str;
    f->priv.fd   = strlen(f->priv.name);
    f->ops = ixp_rostring_ops;
    return 0;
}

long mixp_rostring_ops_size(MIXPSRV_FILE_HANDLE* f)
{
    return f->priv.fd;
}

int mixp_rostring_ops_stat(MIXPSRV_FILE_HANDLE* f, MIXP_STAT* s)
{
    fprintf(stderr,"ixp_rostring_ops_size() stat not implemented yet\n");
    return 0;
}
