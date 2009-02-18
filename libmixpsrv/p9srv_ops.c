
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <inttypes.h>

#include <9p-mixp/mixp.h>
#include <9p-mixp/convert.h>
#include <9p-mixpsrv/types.h>
#include <9p-mixpsrv/p9srv.h>
#include <9p-mixpsrv/prototypes.h>
#include <9p-mixpsrv/errors.h>
#include <9p-mixpsrv/call_ops.h>
#include <9p-mixpsrv/errors.h>

/* Macros */
#define QID(t, i) (((long long)((t)&0xFF)<<32)|((i)&0xFFFFFFFF))

MIXPSRV_FILE_HANDLE* free_fileid;

/* This function's name belies it's true purpose. It increases
 * the reference counts of the File_Handle list */
static void p9srv_clone_files(MIXPSRV_FILE_HANDLE *f) {
	for(; f; f=f->next)
		assert(f->nref++);
}

void p9srv_ops_walk(Ixp9Req *r) 
{
	MIXPSRV_FILE_HANDLE *f, *nf;
	int i;

	f = r->fid->aux;
	p9srv_clone_files(f);
	for(i=0; i < r->ifcall->Twalk.nwname; i++) 
	{
		if(!strncmp(r->ifcall->Twalk.wname[i], "..", 3))
		{
			if(f->next) {
				nf=f;
				f=f->next;
				p9srv_free_file(nf);
			}
		}
		else
		{
			nf = NULL;
			if (f->ops.lookup)
			    nf = f->ops.lookup(f,r->ifcall->Twalk.wname[i]);

			if(!nf)
			{
				fprintf(stderr,"p9srv_ops_lookup() could not get file \"%s\"\n", r->ifcall->Twalk.wname[i]);
				break;
			}
			assert(!nf->next);
			if(strncmp(r->ifcall->Twalk.wname[i], ".", 2)) {
				nf->next = f;
				f = nf;
			}
		}
		r->ofcall->Rwalk.wqid[i].type = f->qtype;
		r->ofcall->Rwalk.wqid[i].path = QID(f->type, f->id);
	}
	/* There should be a way to do this on freefid() */
	if(i < r->ifcall->Twalk.nwname) {
		while((nf = f)) {
			f=f->next;
			p9srv_free_file(nf);
		}
		ixp_respond(r, MIXPSRV_ERR_NOFILE);
		return;
	}
	/* Remove refs for r->fid if no new fid */
	/* If Fids were ref counted, this could be
	 * done in their decref function */
	if(r->ifcall->fid == r->ifcall->Twalk.newfid) {
		nf=r->fid->aux;
		r->fid->aux = f;
		while((nf = f)) {
			f=f->next;
			p9srv_free_file(nf);
		}
	}
	r->newfid->aux = f;
	r->ofcall->Rwalk.nwqid = i;
	ixp_respond(r, NULL);
}

unsigned int p9srv_ops_size(MIXPSRV_FILE_HANDLE *f) 
{
	return mixpsrv_call_ops_size(f);
}

/* This function needs to be seriously cleaned up */
void p9srv_ops_write(Ixp9Req *r) 
{
	MIXPSRV_FILE_HANDLE *f = r->fid->aux;
	int ret = mixpsrv_call_ops_write(f,r->ifcall->Twrite.offset,r->ifcall->Twrite.count,r->ifcall->Twrite.data);

	if (ret<0)
	{
	    errno = -ret;
	    perror("f->ops.write() returned an error: ");
	    r->ofcall->Rwrite.count = 0;
	    ixp_respond(r,MIXPSRV_ERR_NOPERM);
	}
	else
	{
	    r->ofcall->Rwrite.count = ret;	    
	    ixp_respond(r, NULL);
	}
	return;

	/* This is an assert because it should this should not be called if
	 * the file is not open for writing. */
	assert(!"Write called on an unwritable file");
}

void p9srv_ops_open(Ixp9Req *r) 
{
	MIXPSRV_FILE_HANDLE *f = r->fid->aux;

//	fprintf(stderr,"p9srv_ops_open() called on class %s\n", f->ops.classname);
//	fprintf(stderr,"requested mode: %d\n", r->ifcall->Topen.mode);
//	fprintf(stderr,"file mode:      %d\n", f->perm);

	int ret;
	
	ret = mixpsrv_call_ops_open(f,r->ifcall->Topen.mode);

	switch (ret)
	{
	    case 0:
		fprintf(stderr,"p9srv_ops_open() sending sending Ok\n");
		ixp_respond(r, NULL);
	    break;
	    case -EPERM:
	    default:
		fprintf(stderr,"p9srv_ops_open(): sending permission denied (ret: %d)\n", ret);
		ixp_respond(r, MIXPSRV_ERR_NOPERM);
	}
}

// FIXME: create is not implemented yet
void p9srv_ops_create(Ixp9Req *r) 
{
//	MIXPSRV_FILE_HANDLE *f = r->fid->aux;

	/* XXX: This should be taken care of by the library */
	ixp_respond(r, MIXPSRV_ERR_NOPERM);
	return;
}

void p9srv_ops_remove(Ixp9Req *r) 
{
	MIXPSRV_FILE_HANDLE *f = r->fid->aux;
	int ret = -EPERM;

	if (f->ops.unlink)
	    ret = f->ops.unlink(f);

	switch (ret)
	{
	    case '0':	 ixp_respond(r,NULL);			break;
	    case -EPERM: ixp_respond(r,MIXPSRV_ERR_NOPERM);	break;
	    default:     ixp_respond(r,MIXPSRV_ERR_NOPERM);	break;
	}
}

void p9srv_ops_clunk(Ixp9Req *r)
{
	// We actually don't have to do anything here. ixp_respond() will take
	// care of all (calling freefid, etc).
	ixp_respond(r, NULL);
}

void p9srv_ops_flush(Ixp9Req *r)
{
	fprintf(stderr,"p9srv_ops_flush: flush not implemented yet\n");
//	Ixp9Req **t;

//	for(t=&pending_event_reads; *t; t=(Ixp9Req **)&(*t)->aux)
//		if(*t == r->oldreq) {
//			*t = (*t)->aux;
//			ixp_respond(r->oldreq, MIXPSRV_ERR_INTERRUPTED);
//			break;
//		}

	ixp_respond(r, NULL);
}

void p9srv_ops_freefid(IxpFid *f)
{
	MIXPSRV_FILE_HANDLE *id, *tid;

	for(id=f->aux; id; id = tid) {
		tid = id->next;
		p9srv_free_file(id);
	}
}

void p9srv_free_file(MIXPSRV_FILE_HANDLE *f)
{
	if(--f->nref)
		return;

	// call the file handle's close operation first
	mixpsrv_call_ops_close(f);

	if (f->name)
	{
		free(f->name);
		f->name = NULL;
	}
	f->next = free_fileid;
	free_fileid = f;
}

void p9srv_ops_stat(Ixp9Req *r)
{
	MIXP_STAT s;
	unsigned int size;
	char *buf;
	MIXP_MESSAGE msg;
	MIXPSRV_FILE_HANDLE * f = (MIXPSRV_FILE_HANDLE*)r->fid->aux;

	fprintf(stderr,"p9srv_ops_stat()\n");

	/* run our actual stat() call */
	mixpsrv_call_ops_stat(f, &s);
	
	/* prepare Rstat ofcall */
	r->ofcall->Rstat.nstat = size = mixp_stat_sizeof(&s);
	r->ofcall->Rstat.stat  = buf  = calloc(1,size);
	
	/* build the message payload */
	msg = mixp_message(buf,size,MsgPack);
	mixp_pstat(&msg, &s);

	/* send response */	
	ixp_respond(r, NULL);
}

void p9srv_ops_read_dir(Ixp9Req *r, MIXPSRV_FILE_HANDLE* f)
{
	MIXPSRV_FILE_HANDLE* tf      = f;
	int         offset  = 0;
	int         size    = 0;
	int         written = 0;
	char *      buf;
	MIXP_MESSAGE      msg;

	printf("reading directory: %s\n", f->name);

	size = r->ifcall->Tread.count;
	buf = calloc(1,size);
	r->ofcall->Rread.data = buf;
	msg = mixp_message(buf,size,MsgPack);

	if (f->ops.lookup)
		tf = f = f->ops.lookup(f, NULL);
	else
		tf = f = NULL;
		
	if (f != NULL)
	{
		/* OLD-Note: f->tab.name == "." so we skip it */
		for(f=tf; f; f=f->next) 
		{
			int n = 0;
		
			// Skip the "." and ".." entries
			if (!(f->name))
			{
				fprintf(stderr,"p9srv_ops_read() broken MIXPSRV_FILE_HANDLE: NULL name\n");
			}
			else if ((strcmp(f->name,".")==0) || (strcmp(f->name,"..")==0))
			{
				fprintf(stderr,"p9srv_ops_read() skipped %s\n", f->name);
			}
			else
			{
				MIXP_STAT s;

				mixpsrv_call_ops_stat(f,&s);

				n = mixp_stat_sizeof(&s);
				if(offset >= r->ifcall->Tread.offset) 
				{
					if ((written+n) > size)
					{
//						fprintf(mixp_debug_stream,"Buffer too small for next element ... client has to do further requests\n");
						break;
					}
					mixp_pstat(&msg,&s);
					written += n;
//					ixp_pack_stat((unsigned char **)&buf, &size, &s);
				}
				else
//					fprintf(stderr,"skipping tread.offs=%lld curoffs=%d n=%d size=%d\n", 
//						r->ifcall->Tread.offset, 
//						offset,
//						n,
//						size);

				offset += n;
			}
		}
		while((f = tf)) 
		{
			tf=tf->next;
			p9srv_free_file(f);
		}
	}
	r->ofcall->Rread.count = written;
	printf("read_dir: written: %d offset=%d\n", written, offset);
	ixp_respond(r, NULL);
}

void p9srv_ops_read_plain(Ixp9Req* r, MIXPSRV_FILE_HANDLE* f)
{
//	MIXPSRV_FILE_HANDLE* tf;
	int ret;
	ssize_t s = 12;

	printf("Test: %u\n", s);
	printf("! Tread.count=%u\n", r->ifcall->Tread.count);
	if (r->ifcall->Tread.count<0)
	{
	    printf("Tread.count < 0 !\n");
	    r->ifcall->Tread.count = 1;
	}

	r->ofcall->Rread.data = calloc(1,r->ifcall->Tread.count);

	ret = mixpsrv_call_ops_read(f,r->ifcall->Tread.offset,r->ifcall->Tread.count,r->ofcall->Rread.data);

	if (ret<0)
	{
		perror("f->ops.read() returned an error: ");
	        r->ofcall->Rread.count = 0;
	}
	else
		r->ofcall->Rread.count = ret;
		    
	ixp_respond(r, NULL);
}

void p9srv_ops_read(Ixp9Req *r) 
{
	MIXPSRV_FILE_HANDLE *f;

	if (r->ifcall->Tread.count < 1)
	{
	    fprintf(stderr,"p9srv_ops_read_plain() count %d too small - tweaking to 1\n", r->ifcall->Tread.count);
	    r->ifcall->Tread.count = 1;
	}
	if (r->ifcall->Tread.count > 32768)
	{
	    fprintf(stderr,"p9srv_ops_read_plain() count %d too large - tweaking to 32768\n", r->ifcall->Tread.count);
	    r->ifcall->Tread.count = 32768;
	}

	f = r->fid->aux;
	if(f->perm & P9_DMDIR && f->perm & 0400) 
	    p9srv_ops_read_dir(r, f);
	else
	    p9srv_ops_read_plain(r, f);
}

MIXPSRV_FILE_HANDLE * p9srv_get_file() 
{
	MIXPSRV_FILE_HANDLE *temp;
	if(!free_fileid) 
	{
		unsigned int i = 15;
		temp = calloc(1,sizeof(MIXPSRV_FILE_HANDLE) * i);
		for(; i; i--) {
			temp->next = free_fileid;
			free_fileid = temp++;
		}
	}
	temp = free_fileid;
	free_fileid = temp->next;
	temp->nref = 1;
	temp->next = NULL;
	temp->ops  = mixp_default_fileops;
	return temp;
}

void p9srv_ops_attach(Ixp9Req *r) 
{	
	printf("p9srv_ops_attach()\n");

	MIXPSRV_FILESERVER* fserv = (MIXPSRV_FILESERVER*)r->srv->aux;
	if (fserv->magic != MIXPSRV_FILESERVER_MAGIC)
	    assert(!"Broken private data for server Ixp9Srv structure");

	if (!fserv->openRoot)
	    assert(!"Missing openRoot() method in Ixp9Srv's private structure");

	MIXPSRV_FILE_HANDLE * f = fserv->openRoot(fserv);

	r->fid->aux = f;
	r->fid->qid.type = f->qtype;
	r->fid->qid.path = QID(f->type, 0);
	r->ofcall->Rattach.qid = r->fid->qid;
	ixp_respond(r, NULL);
}

int p9srv_run_server(int fd, MIXPSRV_FILESERVER* fspriv)
{
	Ixp9Srv   my_p9srv = p9srv_ops;
	MIXP_SERVER my_srv;

	fspriv->magic = MIXPSRV_FILESERVER_MAGIC;
	my_p9srv.aux = fspriv;
	ixp_listen(&my_srv, fd, &my_p9srv, serve_9pcon, NULL);
	ixp_serverloop(&my_srv);
	return 0;
}

Ixp9Srv p9srv_ops = {
	.open    = p9srv_ops_open,
	.walk    = p9srv_ops_walk,
	.read    = p9srv_ops_read,
	.stat    = p9srv_ops_stat,
	.write   = p9srv_ops_write,
	.clunk   = p9srv_ops_clunk,
	.flush   = p9srv_ops_flush,
	.attach  = p9srv_ops_attach,
	.create  = p9srv_ops_create,
	.remove  = p9srv_ops_remove,
	.freefid = p9srv_ops_freefid
};

MIXPSRV_FILE_HANDLE* p9srv_get_dir(const char* name, const char* priv_name, MIXPSRV_FILE_OPS ops)
{
    MIXPSRV_FILE_HANDLE* f = p9srv_get_file();
    if (!f)
    {
	fprintf(stderr,"EMERG: p9srv_get_dir() could not allocate MIXPSRV_FILE_HANDLE ... out of memory ?\n");
	return NULL;
    }
    if (name)
        f->name=strdup(name);
    else
    {
	fprintf(stderr,"WARN: p9srv_get_dir() missing name!\n");
	f->name=strdup("");
    }

    f->qtype     = P9_QTDIR;
    f->type      = 0;
    f->perm      = 0555|P9_DMDIR;
    f->priv.name = priv_name;
    f->ops       = ops;

    return f;
}
