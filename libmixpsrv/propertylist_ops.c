
// #define _DEBUG 

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <9p-mixpsrv/types.h>
#include <9p-mixpsrv/p9srv.h>
#include <9p-mixpsrv/propertylist_ops.h>

#define PROPLISTPRIV(f)		((MIXP_PROPERTYLIST_DEF*)(f->priv.name))
#define PROPENTPRIV(f)		((MIXP_PROPERTYLIST_DEF*)(f->priv.text))

static MIXPSRV_FILE_HANDLE* mixp_propertylist_ops_lookup  (MIXPSRV_FILE_HANDLE* f, char* name);
static int                  mixp_propertylist_ops_stat    (MIXPSRV_FILE_HANDLE* f, MIXP_STAT* stat);
static long                 mixp_propertylist_ops_size    (MIXPSRV_FILE_HANDLE* f);
static int                  mixp_propertylist_ops_close   (MIXPSRV_FILE_HANDLE* f);
static long                 mixp_propertylist_entops_read (MIXPSRV_FILE_HANDLE* f, long offset, size_t size, void* buf);
static long                 mixp_propertylist_entops_write(MIXPSRV_FILE_HANDLE* f, long offset, size_t size, void* buf);
static int                  mixp_propertylist_entops_open (MIXPSRV_FILE_HANDLE* f, long mode);
static int                  mixp_propertylist_entops_stat (MIXPSRV_FILE_HANDLE* f, MIXP_STAT* stat);
static long                 mixp_propertylist_entops_size (MIXPSRV_FILE_HANDLE* f);
static int                  mixp_propertylist_entops_close(MIXPSRV_FILE_HANDLE* f);

static MIXPSRV_FILE_OPS propertylist_ops = 
{
    .lookup = mixp_propertylist_ops_lookup,
    .stat   = mixp_propertylist_ops_stat,
    .size   = mixp_propertylist_ops_size,
    .close  = mixp_propertylist_ops_close
};

static MIXPSRV_FILE_OPS propertylist_entops = 
{
    .read  = mixp_propertylist_entops_read,
    .write = mixp_propertylist_entops_write,
    .open  = mixp_propertylist_entops_open,
    .stat  = mixp_propertylist_entops_stat,
    .size  = mixp_propertylist_entops_size,
    .close = mixp_propertylist_entops_close
};

MIXPSRV_FILE_HANDLE* mixp_propertylist_create(const char* name, MIXP_PROPERTYLIST_DEF* pr)
{
#ifdef _DEBUG
    printf("mixp_propertylist_create() name=\"%s\"\n", name);
#endif
    MIXPSRV_FILE_HANDLE* f = p9srv_get_file();
    f->name      = strdup(name);
    f->qtype     = P9_QTDIR;
    f->type      = 0;
    f->perm      = 0555|P9_DMDIR;
    f->priv.name = (char*)pr;
    f->ops       = propertylist_ops;
    return f;
}

static MIXPSRV_FILE_HANDLE* mixp_propertylist_createent(MIXP_PROPERTYLIST_DEF* pr, MIXP_PROPERTYLIST_ENT* ent)
{
#ifdef _DEBUG
    printf("mixp_propertylist_createent() name=\"%s\"\n", ent->name);
#endif
    if (ent->getFileHandle)
	return ent->getFileHandle(pr,ent);

    MIXPSRV_FILE_HANDLE* f = p9srv_get_file();
    f->name      = strdup(ent->name);
    f->qtype     = P9_QTFILE;
    f->type      = 0;
    f->perm      = 0555;
    f->priv.name = (char*)pr;
    f->priv.text = (char*)ent;
    f->ops       = propertylist_entops;
    return f;
}

// FIXME: not really necessary
static int  mixp_propertylist_ops_close(MIXPSRV_FILE_HANDLE* f)
{
    fprintf(stderr,"mixp_propertylist_ops_close()\n");
    return 1;
}

static long mixp_propertylist_ops_size(MIXPSRV_FILE_HANDLE *f)
{
#ifdef _DEBUG
    fprintf(stderr,"mixp_propertylist_ops_size() DUMMY\n");
#endif
    return 0;
}

static inline MIXPSRV_FILE_HANDLE* mixp_propertylist_ops_lookup_list(MIXP_PROPERTYLIST_DEF* def)
{
    int x;
    MIXPSRV_FILE_HANDLE* fh = NULL;
    MIXPSRV_FILE_HANDLE* f  = NULL;
    for (x=0; def->entries[x].name; x++)
    {
#ifdef _DEBUG
	fprintf(stderr,"MIXP_PROPERTYLIST_OPS_lookup_list() walking: \"%s\"\n", def->entries[x].name);
#endif
	if (fh==NULL)
	{
	    f = fh = mixp_propertylist_createent(def,&(def->entries[x]));
	}
	else
	{
	    fh->next = mixp_propertylist_createent(def,&(def->entries[x]));
	    fh = fh->next;
	}
    }

    return f;
}

static inline MIXPSRV_FILE_HANDLE* mixp_propertylist_ops_lookup_ent(MIXP_PROPERTYLIST_DEF* def, const char* name)
{
    int x;
    for (x=0; def->entries[x].name; x++)
    {
	if (strcmp(def->entries[x].name,name)==0)
	{
	    return mixp_propertylist_createent(def, &(def->entries[x]));
	}
    }
    return NULL;
}

static MIXPSRV_FILE_HANDLE* mixp_propertylist_ops_lookup(MIXPSRV_FILE_HANDLE* f, char* name)
{
#ifdef _DEBUG
    printf("mixp_propertylist_ops_lookup()\n");
#endif
    if (name==NULL)
	return mixp_propertylist_ops_lookup_list(PROPLISTPRIV(f));
    else
	return mixp_propertylist_ops_lookup_ent(PROPLISTPRIV(f),name);
}

static inline MIXPSRV_IORESULT _write_intval(MIXP_PROPERTYLIST_DEF* pr, MIXP_PROPERTYLIST_ENT* ent, const char* text)
{
    long l = -666;
    errno = 0;
    l = strtol(text,NULL,0);

    if (errno!=0)
    {
	fprintf(stderr,"ixp_propertylist_write_long(): hmm, could not parse long out of text \"%s\" - sending EPERM\n", text);
	perror("conversion failed: ");
	return ((MIXPSRV_IORESULT){.err = EPERM });
    }

    ent->int_value = l;
    if (pr->ops.setInt)
    {
	long res = pr->ops.setInt(pr,ent,l);
	if (res<0)
	    return ((MIXPSRV_IORESULT){.err = -res });
	else
	    return ((MIXPSRV_IORESULT){.err = 0, .size = strlen(text)});
    }
    else
	return ((MIXPSRV_IORESULT){.err = 0, .size = strlen(text) });
}

static inline MIXPSRV_IORESULT _write_string(MIXP_PROPERTYLIST_DEF* pr, MIXP_PROPERTYLIST_ENT* ent, const char* text)
{
    if (pr->ops.setString == NULL)
    {
#ifdef _DEBUG
	fprintf(stderr,"ixp_propertylist_write_string(): Propertylist has no setInt() method. Sending EPERM\n");
#endif	
	return ((MIXPSRV_IORESULT){.err = EPERM, .size = 0});
    }
#ifdef _DEBUG
    fprintf(stderr,"ixp_propertylist_write_string() writing string \"%s\"\n", text);
#endif
    int ret = pr->ops.setString(pr,ent,text);
    if (ret<0)
	return ((MIXPSRV_IORESULT){.err = -ret, .size = 0});
    else
	return ((MIXPSRV_IORESULT){.err = 0, .size = ret});
}

static long mixp_propertylist_entops_write(MIXPSRV_FILE_HANDLE* f, long offset, size_t size, void* buf)
{
    if (size<1)
    {
#ifdef _DEBUG
	fprintf(stderr,"DEBUG ixp_propertylist_entops_write() ignoring empty write (kind of EOF ?)\n");
#endif
	return 0;
    }

    MIXP_PROPERTYLIST_DEF * def = (MIXP_PROPERTYLIST_DEF*) f->priv.name;
    MIXP_PROPERTYLIST_ENT * ent = (MIXP_PROPERTYLIST_ENT*) f->priv.text;

#ifdef _DEBUG
    fprintf(stderr,"ixp_propertylist_entops_write() offset=%ld size=%ld ptr=%ld\n", offset, size, buf);
    fprintf(stderr," -> name=\"%s\"\n", ent->name);
#endif

    if (buf==NULL)
    {
	fprintf(stderr,"ERROR: ixp_propertylist_entops_write() NULL buffer !\n");
	return -EPERM;
    }
#ifdef _DEBUG
    fprintf(stderr,"ixp_propertylist_entops_write() offset=%ld size=%u\n", offset, size);
#endif
    // convert counted-size memchunk to 0-terminated char
    char* text = calloc(1,size+1);
    memset(text,0,size+1);
    memcpy(text,buf,size);

    if (offset)
    {
	fprintf(stderr,"FIXME: ixp_propertylist_entops_write() offset=%ld > 0 - not supported. sending EPERM\n", offset);
	return -EPERM;
    }
    
    MIXPSRV_IORESULT res;
    
    switch (ent->type)
    {
	case P9_PL_INT:		res = _write_intval(def, ent, text);	break; 
	case P9_PL_INT_VAL:	res = _write_intval(def, ent, text); 	break;
	case P9_PL_STRING:	res = _write_string(def, ent, text);	break;
	default:
	    fprintf(stderr,"ixp_propertylist_entops_write() unsupported ent type %ud - sending EPERM\n", ent->type);
	    return -EPERM;
    }
    
    if (res.err != 0)
	return -res.err;
    else
	return res.size;
}

static long _entops_read_intval(MIXP_PROPERTYLIST_DEF* def, MIXP_PROPERTYLIST_ENT* ent, long offset, size_t size, void* buf)
{
    if (offset)
    {
	fprintf(stderr,"read_int() does not support offsets (%ld)\n", offset);
	return 0;
    }

    // evrything seems okay - get 
    long ret;
    if (def->ops.getInt==NULL)
    {
	if (ent->value)
	{
	    strncpy(buf,ent->value,size-1);
	    return strlen(buf);
	}
	ret = ent->int_value;
    }
    else
	def->ops.getInt(def,ent,&ret);
    return snprintf(buf,size-1,"%ld",ret);
}

static long _entops_read_int(MIXP_PROPERTYLIST_DEF* def, MIXP_PROPERTYLIST_ENT* ent, long offset, size_t size, void* buf)
{
    // check if we have an constant value
    if (ent->value)
    {
	strncpy(buf,ent->value,size-1);
	return strlen(buf);
    }

    if (offset)
    {
	fprintf(stderr,"read_int() does not support offsets (%ld)\n", offset);
	return 0;
    }

    // do we have an getInt() handler in the propertylist ?
    if (def->ops.getInt==NULL)
    {
	fprintf(stderr,"read_int() propertylist has no getInt() handler\n");
	return 0;
    }

    // evrything seems okay - get 
    long ret;
    def->ops.getInt(def,ent,&ret);
    return snprintf(buf,size-1,"%ld",ret);
}

static long _entops_read_dir(MIXP_PROPERTYLIST_DEF* def, MIXP_PROPERTYLIST_ENT* ent, long offset, size_t size, void* buf)
{
    fprintf(stderr,"Reading text from directories not supported yet\n");
    return 0;
}

static long _entops_read_string(MIXP_PROPERTYLIST_DEF* def, MIXP_PROPERTYLIST_ENT* ent, long offset, size_t size, void* buf)
{
#ifdef _DEBUG
    printf("Reading from String\n");
#endif
    // we've got an constant value
    if (ent->value)
    {
	// FIXME: should handle offsets !
	if (offset)
	{
	    fprintf(stderr,"read_string() does not yet support offsets in consts (%ld)\n", offset);
	    return 0;
	}
	strncpy(buf,ent->value,size-1);
	return strlen(buf);
    }

    // no constant value -- check for getString() handler
    if (def->ops.getString == NULL)
    {
#ifdef _DEBUG
	fprintf(stderr,"read_string() no getString handler in this propertylist\n");
#endif
	strcpy(buf,"");
	return 0;
    }

    // offsets are not yet support :(
    if (offset)
    {
	fprintf(stderr,"read_string() does not support offsets (%ld)\n", offset);
	return 0;
    }

    // evrything seems okay - call the getString() handler
#ifdef _DEBUG
    fprintf(stderr,"reading string\n");
#endif	    
    def->ops.getString(def,ent,buf,size);
    return strlen(buf);
}

// FIMXE: add sanity checks
static long mixp_propertylist_entops_read(MIXPSRV_FILE_HANDLE* f, long offset, size_t size, void* buf)
{
    MIXP_PROPERTYLIST_DEF * def = (MIXP_PROPERTYLIST_DEF*) f->priv.name;
    MIXP_PROPERTYLIST_ENT * ent = (MIXP_PROPERTYLIST_ENT*) f->priv.text;
#ifdef _DEBUG
    fprintf(stderr,"ixp_propertylist_entops_read() reading from \"%s\" offset %ld size %u\n", ent->name, offset, size);
#endif
    if (buf == NULL)
    {
	fprintf(stderr,"mixp_propertylist_entops_read() Buffer is NULL\n");
	return 0;
    }

    // the buffer is *always* zero'ed first. our clients can rely on that
    memset(buf,0,size);
    switch (ent->type)
    {
	case P9_PL_STRING:	return _entops_read_string (def, ent, offset, size, buf);
	case P9_PL_INT:		return _entops_read_int    (def, ent, offset, size, buf);
	case P9_PL_INT_VAL:	return _entops_read_intval (def, ent, offset, size, buf);
	case P9_PL_DIR:		return _entops_read_dir    (def, ent, offset, size, buf);
	default:
	    fprintf(stderr,"ixp_propertylist_entops_read() unsupported entry type %ud\n", ent->type);
	    return 0;
    }

    return 0;
}

static long mixp_propertylist_entops_size(MIXPSRV_FILE_HANDLE* f)
{
    char buffer[1024];
    buffer[0] = 0;
    mixp_propertylist_entops_read(f, 0, sizeof(buffer)-1, buffer);
    return strlen(buffer);
}

static int mixp_propertylist_entops_close(MIXPSRV_FILE_HANDLE* f)
{
    fprintf(stderr,"mixp_propertylist_entops_close()\n");
    if (f->name)
    {
	free(f->name);
	f->name = NULL;
    }
    return 1;
}

static int mixp_propertylist_entops_open(MIXPSRV_FILE_HANDLE* f, long mode)
{
#ifdef _DEBUG
    MIXP_PROPERTYLIST_DEF * def = (MIXP_PROPERTYLIST_DEF*) f->priv.name;
    MIXP_PROPERTYLIST_ENT * ent = (MIXP_PROPERTYLIST_ENT*) f->priv.text;

    fprintf(stderr,"ixp_propertylist_entops_open() mode %ld\n", mode);
    fprintf(stderr,"ixp_propertylist_entops_open() \"%s\" mode %ld\n", ent->name, mode);
#endif
    return 0;
}

static int mixp_propertylist_ops_stat (MIXPSRV_FILE_HANDLE* f, MIXP_STAT* stat)
{
    MIXP_PROPERTYLIST_DEF * def = (MIXP_PROPERTYLIST_DEF*) f->priv.name;

    mixpsrv_default_ops_stat(f,stat);

    
    if (def->uid)
	stat->uid = strdup(def->uid);

    if (def->gid)
	stat->gid = strdup(def->gid);

    return 1;
}

static int mixp_propertylist_entops_stat (MIXPSRV_FILE_HANDLE* f, MIXP_STAT* stat)
{
    MIXP_PROPERTYLIST_DEF * def = (MIXP_PROPERTYLIST_DEF*) f->priv.name;
    MIXP_PROPERTYLIST_ENT * ent = (MIXP_PROPERTYLIST_ENT*) f->priv.text;

    mixpsrv_default_ops_stat(f,stat);
    // remove execute flag from mode field
    // FIXME: we should have an field in the propertylist def for that ;-o
    if (stat->mode &  S_IXUSR)
	stat->mode -= S_IXUSR;
    if (stat->mode &  S_IXGRP)
	stat->mode -= S_IXGRP;
    if (stat->mode &  S_IXOTH)
	stat->mode -= S_IXOTH;

    if (ent->uid)
	stat->uid = strdup(ent->uid);
    else if (def->uid)
	stat->uid = strdup(def->uid);

    if (ent->gid)
	stat->gid = strdup(ent->gid);
    else if (def->gid)
	stat->gid = strdup(def->gid);

    return 1;
}

MIXP_PROPERTYLIST_DEF* mixp_propertylist_create_def(
    MIXP_PROPERTYLIST_ENT* entries, 
    MIXP_PROPERTYLIST_OPS ops,
    const char* uid,
    const char* gid,
    void* private
)
{
    MIXP_PROPERTYLIST_DEF* def = (MIXP_PROPERTYLIST_DEF*)malloc(sizeof(MIXP_PROPERTYLIST_DEF));
    memset(def,0,sizeof(MIXP_PROPERTYLIST_DEF));
    def->entries = entries;
    def->ops     = ops;
    def->priv    = private;
    def->uid     = strdup(uid);
    def->gid     = strdup(gid);
    return def;
}
