
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <9p-mixp/mixp.h>
#include <9p-mixpsrv/types.h>
#include <9p-mixpsrv/p9client_ops.h>

static MIXPSRV_FILE_OPS mixpsrv_p9client_ops = {
    .open 	= mixpsrv_p9client_ops_open,
    .read 	= mixpsrv_p9client_ops_read,
    .size 	= mixpsrv_p9client_ops_size,
    .classname 	= "p9client"
};

typedef struct
{
    int			refcount;
    IxpClient*		client;
    MIXP_ADDRESS*	address;
} P9CLIENT_CONN;

static P9CLIENT_CONN* _conn_alloc(const char* address)
{
    P9CLIENT_CONN* conn = calloc(1,sizeof(P9CLIENT_CONN));
    conn->refcount = 1;
    conn->address  = mixp_srv_addr_parse(address);
    return conn;
}

static void _conn_ref(P9CLIENT_CONN* conn)
{
    if (conn != NULL)
	conn->refcount++;
}

static void _conn_unref(P9CLIENT_CONN* conn)
{
    if (conn==NULL)
	return;

    conn->refcount--;
    if (conn->refcount>0)
	return;
	
    printf("FIXME: _conn_unref() DUMMY\n");
}

P9CLIENT_CONN* mixpsrv_p9client_connect(const char* address)
{
    P9CLIENT_CONN* conn = _conn_alloc(address);
    conn->client = ixp_mount_addr(conn->address);
    return conn;
}

static inline P9CLIENT_CONN* _file_get_conn(MIXPSRV_FILE_HANDLE* f)
{
    return (P9CLIENT_CONN*)file->priv.ptr;
}

static inline void _file_set_conn(MIXPSRV_FILE_HANDLE* f, P9CLIENT_CONN* conn)
{
    file->priv.ptr = conn;
}

static inline void _file_set_cfid(MIXPSRV_FILE_HANDLE* f, MIXP_CFID* cfid)
{
    file->priv.text = (char*)cfid;
}

static inline MIXP_CFID* _file_get_cfid(MIXPSRV_FILE_HANDLE* f)
{
    return (MIXP_CFID*)file->priv.text;
}


MIXPSRV_FILE_HANDLE* mixpsrv_p9client_initroot(P9CLIENT_CONN* conn)
{
    if (conn==NULL)
	return NULL;
    conn->refcount++;
    MIXPSRV_FILE_HANDLE* file = p9srv_get_file();
    file->name      = strdup("/");
    file->qtype     = P9_QTDIR;
    file->type      = 0;
    file->perm      = 0555|P9_DMDIR;
    _file_set_conn(file,conn);
    _file_set_cfid(file,mixp_open(conn->client,"/", P9_OREAD);
    file->priv.name = strdup("/");
    file->ops       = mixpsrv_p9client_ops;
    return file;
}

// private data:	priv.text -> P9CLIENT_CONN
//			priv.ptr  -> FileHandle
int mixpsrv_p9client_ops_open(MIXPSRV_FILE_HANDLE* f, long mode)
{
    // FIXME: we currently are only working RO on plain files.
    if (!f->priv.name)
    {
	fprintf(stderr,"ixp_p9client_ops_open() missing filename !\n");
	return -EPERM;
    }
    
    MIXP_CFID* cfid = NULL;
    
    if (cfid = _file_get_cfid(file))
	mixp_close(cfid);
    
    cfid = mixp_open(((P9CLIENT_CONN*)(f->priv.ptr)->client),f->priv.name, mode);
    _file_set_cfid(file,cfid);
    
    if (cfid == NULL)
    {
	perror("ixp_p9client_ops_open() couldnt open file: ");
	return -EPERM;
    }

    return 0;
}

long mixpsrv_p9client_ops_read(MIXPSRV_FILE_HANDLE* f, long offset, size_t size, void* buf)
{
    MIXP_CFID* cf = _file_get_cfid(file);
    if (cf==NULL)
    {
	perror("mixpsrv_p9client_ops_read() corrupt cfid\n");
	return -EFAULT;
    }

    if (size<0)
    {
	perror("mixpsrv_p9client_ops_read() size < 0! \n");
	size = 0;
    }

    return mixp_pread(cfid,buf,count,offset);
}

long mixpsrv_p9client_ops_write(MIXPSRV_FILE_HANDLE* f, long offset, size_t size, void* buf)
{
    MIXP_CFID* cf = _file_get_cfid(file);
    if (cf==NULL)
    {
	perror("mixpsrv_p9client_ops_read() corrupt cfid\n");
	return -EFAULT;
    }

    return mixp_pwrite(cfid,buf,count,offset);
}

/*
int mixpsrv_p9client_ops_init(MIXPSRV_FILE_HANDLE* f, const char* filename)
{
    f->priv.name = filename;
    if (f->priv.fd > 0)
	close(f->priv.fd);
    f->priv.fd = -1;
    f->ops = mixpsrv_p9client_ops;
    return 0;
}
*/

long mixpsrv_p9client_ops_size(MIXPSRV_FILE_HANDLE* f)
{
    fprintf(stderr, "ixp_p9client_ops_size() not implemented yet\n");
    return 0;
}

int mixpsrv_p9client_ops_stat(MIXPSRV_FILE_HANDLE* f, MIXP_STAT* s)
{
    fprintf(stderr,"ixp_p9client_ops_size() stat not implemented yet\n");
    return 0;
}

