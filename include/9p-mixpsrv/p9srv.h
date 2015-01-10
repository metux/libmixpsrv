
#ifndef __IXPSERV_REQ_OPS_H
#define __IXPSERV_REQ_OPS_H

#include <9p-mixp/mixp.h>
#include <9p-mixpsrv/types.h>
#include <9p-mixpsrv/default_ops.h>
#include <9p-mixpsrv/hostfile_ops.h>

/* server operations - they're not much more than mux'es */
/* fs.c */
extern void p9srv_ops_attach(MIXP_REQUEST *r);
extern void p9srv_ops_clunk(MIXP_REQUEST *r);
extern void p9srv_ops_create(MIXP_REQUEST *r);
extern void p9srv_ops_flush(MIXP_REQUEST *r);
extern void p9srv_ops_freefid(MIXP_FID *f);
extern void p9srv_ops_open(MIXP_REQUEST *r);
extern void p9srv_ops_read(MIXP_REQUEST *r);
extern void p9srv_ops_remove(MIXP_REQUEST *r);
extern void p9srv_ops_stat(MIXP_REQUEST *r);
extern void p9srv_ops_walk(MIXP_REQUEST *r);
extern void p9srv_ops_write(MIXP_REQUEST *r);

// extern void p9srv_clone_files(MIXPSRV_FILE_HANDLE *f);
extern void p9srv_free_file(MIXPSRV_FILE_HANDLE*f);
extern MIXPSRV_FILE_HANDLE* p9srv_get_file();
extern MIXPSRV_FILE_HANDLE* p9srv_get_dir(const char* name, const char* priv_name, MIXPSRV_FILE_OPS ops);

int p9srv_run_server(int fd, MIXPSRV_FILESERVER* fspriv);

extern MIXP_SRV_OPS p9srv_ops;

#endif
