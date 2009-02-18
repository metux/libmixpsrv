
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <9p-mixpsrv/types.h>
#include <9p-mixpsrv/hostfile_ops.h>

static MIXPSRV_FILE_OPS mixpsrv_hostfile_ops = {
    .open 	= mixpsrv_hostfile_ops_open,
    .read 	= mixpsrv_hostfile_ops_read,
    .size 	= mixpsrv_hostfile_ops_size,
    .classname 	= "hostfile"
};

int mixpsrv_hostfile_ops_open(MIXPSRV_FILE_HANDLE* f, long mode)
{
    // FIXME: we currently are only working RO on plain files.
    if (!f->priv.name)
    {
	fprintf(stderr,"ixp_hostfile_ops_open() missing filename !\n");
	return -EPERM;
    }
    
    f->priv.fd = open(f->priv.name,O_RDONLY);
    if (f->priv.fd==-1)
    {
	perror("ixp_hostfile_ops_open() couldnt open file: ");
	return -EPERM;
    }
    
    return 0;
}

long mixpsrv_hostfile_ops_read(MIXPSRV_FILE_HANDLE* f, long offset, size_t size, void* buf)
{
    lseek(f->priv.fd, offset, SEEK_SET);
    return read(f->priv.fd, buf, size);
}

int mixpsrv_hostfile_ops_init(MIXPSRV_FILE_HANDLE* f, const char* filename)
{
    f->priv.name = filename;
    if (f->priv.fd > 0)
	close(f->priv.fd);
    f->priv.fd = -1;
    f->ops = mixpsrv_hostfile_ops;
    return 0;
}

long mixpsrv_hostfile_ops_size(MIXPSRV_FILE_HANDLE* f)
{
    fprintf(stderr, "ixp_hostfile_ops_size() not implemented yet\n");
    return 0;
}

int mixpsrv_hostfile_ops_stat(MIXPSRV_FILE_HANDLE* f, MIXP_STAT* s)
{
    fprintf(stderr,"ixp_hostfile_ops_size() stat not implemented yet\n");
    return 0;
}
