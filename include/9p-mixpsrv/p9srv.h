
#ifndef __IXPSERV_REQ_OPS_H
#define __IXPSERV_REQ_OPS_H

#include <9p-mixp/mixp.h>
#include <9p-mixpsrv/types.h>
#include <9p-mixpsrv/default_ops.h>
#include <9p-mixpsrv/hostfile_ops.h>

/* server operations - they're not much more than mux'es */
/* fs.c */
extern void p9srv_ops_attach(Ixp9Req *r);
extern void p9srv_ops_clunk(Ixp9Req *r);
extern void p9srv_ops_create(Ixp9Req *r);
extern void p9srv_ops_flush(Ixp9Req *r);
extern void p9srv_ops_freefid(IxpFid *f);
extern void p9srv_ops_open(Ixp9Req *r);
extern void p9srv_ops_read(Ixp9Req *r);
extern void p9srv_ops_remove(Ixp9Req *r);
extern void p9srv_ops_stat(Ixp9Req *r);
extern void p9srv_ops_walk(Ixp9Req *r);
extern void p9srv_ops_write(Ixp9Req *r);

// extern void p9srv_clone_files(MIXPSRV_FILE_HANDLE *f);
extern void p9srv_free_file(MIXPSRV_FILE_HANDLE*f);
extern MIXPSRV_FILE_HANDLE* p9srv_get_file();
extern MIXPSRV_FILE_HANDLE* p9srv_get_dir(const char* name, const char* priv_name, MIXPSRV_FILE_OPS ops);

int p9srv_run_server(int fd, MIXPSRV_FILESERVER* fspriv);

extern Ixp9Srv p9srv_ops;

#endif

