
#include <unistd.h>
#include <9p-mixpsrv/server.h>
#include <9p-mixpsrv/types.h>
#include <9p-mixpsrv/p9srv.h>

const char* mixp_9pserver_run_tcp(
    const char* addr, 
    int port,
    MIXPSRV_FILE_HANDLE*(*openroot)(),
    void* priv
    )
{
    char* errstr = NULL;
    MIXPSRV_FILESERVER fileserv = {
	.magic		= MIXPSRV_FILESERVER_MAGIC,
	.openRoot	= openroot
    };

    int fd = ixp_serversock_tcp(addr, port, &errstr);
    if (fd<0)
    {
	return errstr;
    }
    p9srv_run_server(fd, &fileserv);
    close(fd);
    return NULL;
}
