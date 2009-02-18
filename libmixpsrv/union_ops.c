
// UNUSABLE FOR NOW !
// will not be built 

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <9p-mixpsrv/types.h>
#include <9p-mixpsrv/p9srv.h>
#include <9p-mixpsrv/propertylist_ops.h>

#define PROPLISTPRIV(f)		((Ixp_Union_Def*)(f->priv.name))
#define PROPENTPRIV(f)		((Ixp_Union_Ent*)(f->priv.text))

#define UNIONENTPRIV(f)		((Ixp_Union_Ent*)(f->priv.text))

typedef struct _union_node UNODE;

struct _union_node
{
    int         refcount;
    char*       name;
    UNODE*      next;
    UNODE*      childs;
    MIXPSRV_FILESERVER*  server;
    MIXPSRV_FILE_HANDLE* parent_handle;
    MIXPSRV_FILE_HANDLE* file_handle;
};

static UNODE* unode_alloc(const char* name)
{
    UNODE* n = calloc(1,sizeof(name));
    n->name = strdup(name);
    n->recount = 1;
    return n;
}

static UNODE* unode_find(UNODE* parent, const char* name)
{
    if (n==NULL)
	return NULL;

    UNODE* walk;
    for (walk=n->childs; walk; walk=walk->next)
	if (strcmp(name,walk->name)==0)
	    return walk;

    return NULL;
}

static int unode_insert(UNODE* parent, UNODE* node)
{
    if (parent==NULL)
	return -EFAULT;
    if (node==NULL)
	return -EINVAL;

    node->next = parent->childs;
    parent->childs = node;

    return 0;
}

// open an file handle for the given unode
MIXPSRV_FILE_HANDLE* unode_get_handle_for_subnode(UNODE* parent, UNODE* child)
{
    MIXPSRV_FILE_HANDLE* fh = p9srv_get_file();
}

// lookup an specific file entry within an given node
MIXPSRV_FILE_HANDLE* unode_ops_lookup_sub(UNODE* node, const char* name)
{
    // look for an matching child node
    UNODE* walk;
    for (walk=node->childs; walk; walk=walk->next)
    {
	if (strcmp(name,walk->name)==0)
	    return unode_get_handle_for_subnode(node, walk);
    }
    
    // found none, so let's look if the parent node has an file handle
    // and try to lookup the desired file there
    if (node->file_handle != NULL)
    {
	MIXPSRV_FILE_HANDLE* sub = mixp_file_lookup(node->file_handle, name);
	if (sub!=NULL)
	    return sub;
    }

    // nothing, again. last try: check for an fileserver an query
    // its root
    if (node->fileserver)
    {
	MIXPSRV_FILE_HANDLE* root = mixp


	{
	    // okay, got one. returning handle with this node
	    
	
	
	}
    }






    MIXPSRV_FILE_HANDLE* f = p9srv_get_file();
    f->name       = strdup(ent->name);
    f->qtype      = P9_QTFILE;
    f->type       = 0;
    f->perm       = 0555;
    f->priv.name  = (char*)pr;
    f->priv.text  = (char*)ent;
    f->ops        = propertylist_ops;
    return f;
}




static MIXPSRV_FILE_HANDLE* ixp_union_ops_lookup (MIXPSRV_FILE_HANDLE* f, char* name);
static long        ixp_union_ops_read   (MIXPSRV_FILE_HANDLE* f, long offset, long size, void* buf);
static long        ixp_union_ops_write  (MIXPSRV_FILE_HANDLE* f, long offset, long size, void* buf);
static int         ixp_union_ops_open   (MIXPSRV_FILE_HANDLE* f, long mode);
static int         ixp_union_ops_stat   (MIXPSRV_FILE_HANDLE* f, MIXP_STAT* stat);
static long        ixp_union_ops_size   (MIXPSRV_FILE_HANDLE* f);

static MIXPSRV_FILE_OPS propertylist_ops = 
{
    .read   = ixp_union_ops_read,
    .write  = ixp_union_ops_write,
    .open   = ixp_union_ops_open,
    .stat   = ixp_union_ops_stat,
    .size   = ixp_union_ops_size,
    .lookup = ixp_union_ops_lookup
};

MIXPSRV_FILE_HANDLE* ixp_union_create(const char* name, Ixp_Union_Def* pr)
{
    MIXPSRV_FILE_HANDLE* f = p9srv_get_file();
    f->name       = strdup(name);
    f->qtype      = P9_QTDIR;
    f->type       = 0;
    f->perm       = 0555|P9_DMDIR;
    f->priv.name  = (char*)pr;
    f->ops        = propertylist_ops;
    return f;
}

static MIXPSRV_FILE_HANDLE* ixp_union_createent(Ixp_Union_Def* pr, Ixp_Union_Ent* ent)
{
    if (ent->getFileHandle)
	return ent->getFileHandle(pr,ent);

    MIXPSRV_FILE_HANDLE* f = p9srv_get_file();
    f->name       = strdup(ent->name);
    f->qtype      = P9_QTFILE;
    f->type       = 0;
    f->perm       = 0555;
    f->priv.name  = (char*)pr;
    f->priv.text  = (char*)ent;
    f->ops        = propertylist_ops;
    return f;
}

static inline MIXPSRV_FILE_HANDLE* ixp_union_ops_lookup_list(MIXPSRV_FILE_HANDLE*f)
{
    Ixp_Union_Ent* ent = UNIONENTPRIV(f);
    
    // look if we have an file handle (fs mounted in)
    MIXPSRV_FILE_HANDLE* childs = ((f.ops.lookup == NULL) ? NULL : f.ops.lookup(f,NULL));

    if (childs!=NULL)
    {
	
    }

    // we've basicly to possible situations:
//    MIXPSRV_FILE_HANDLE* sublist
    
    int x;
    MIXPSRV_FILE_HANDLE* fh = NULL;
    MIXPSRV_FILE_HANDLE* f  = NULL;
    for (x=0; def->entries[x].name; x++)
    {
	fprintf(stderr,"ixp_union_ops_lookup_list() walking: \"%s\"\n", def->entries[x].name);
	if (fh==NULL)
	{
	    f = fh = ixp_union_createent(def,&(def->entries[x]));
	}
	else
	{
	    fh->next = ixp_union_createent(def,&(def->entries[x]));
	    fh = fh->next;
	}
    }

    return f;
}

static inline MIXPSRV_FILE_HANDLE* ixp_union_ops_lookup_ent(Ixp_Union_Def* def, const char* name)
{
    int x;
    for (x=0; def->entries[x].name; x++)
    {
	if (strcmp(def->entries[x].name,name)==0)
	{
	    return ixp_union_createent(def, &(def->entries[x]));
	}
    }
    return NULL;
}

static MIXPSRV_FILE_HANDLE* ixp_union_ops_lookup(MIXPSRV_FILE_HANDLE* f, char* name)
{
    if (name==NULL)
	return ixp_union_ops_lookup_list(f);
    else
	return ixp_union_ops_lookup_ent(PROPLISTPRIV(f),name);
}

static inline long ixp_union_write_long(Ixp_Union_Def* pr, Ixp_Union_Ent* ent, const char* text)
{
    if (pr->ops.setInt == NULL)
    {
	fprintf(stderr,"ixp_union_write_long(): Propertylist has no setInt() method. Sending EPERM\n");
	return -EPERM;
    }

    long l = -666;
    errno = 0;
    l = strtol(text,NULL,0);
    
    if (errno!=0)
    {
	fprintf(stderr,"ixp_union_write_long(): hmm, could not parse long out of text \"%s\" - sending EPERM\n", text);
	perror("conversion failed: ");
	return -EPERM;
    }

    return pr->ops.setInt(pr,ent,l);
}

static inline long ixp_union_write_string(Ixp_Union_Def* pr, Ixp_Union_Ent* ent, const char* text)
{
    if (pr->ops.setString == NULL)
    {
	fprintf(stderr,"ixp_union_write_string(): Propertylist has no setInt() method. Sending EPERM\n");
	return -EPERM;
    }

    fprintf(stderr,"ixp_union_write_string() writing string \"%s\"\n", text);
    return pr->ops.setString(pr,ent,text);
}

static long ixp_union_ops_write(MIXPSRV_FILE_HANDLE* f, long offset, long size, void* buf)
{
    if (size==0)
    {
//	fprintf(stderr,"DEBUG ixp_union_ops_write() ignoring empty write (kind of EOF ?)\n");
	return 0;
    }

    Ixp_Union_Def * def = (Ixp_Union_Def*) f->priv.name;
    Ixp_Union_Ent * ent = (Ixp_Union_Ent*) f->priv.text;

//    fprintf(stderr,"ixp_union_ops_write() offset=%ld size=%ld ptr=%ld\n", offset, size, buf);
//    fprintf(stderr," -> name=\"%s\"\n", ent->name);

    if (buf==NULL)
    {
	fprintf(stderr,"ERROR: ixp_union_ops_write() NULL buffer !\n");
	return -EPERM;
    }

    fprintf(stderr,"ixp_union_ops_write() offset=%ld size=%ld\n", offset, size);

    // convert counted-size memchunk to 0-terminated char
    char* text = malloc(size+1);
    memset(text,0,size+1);
    memcpy(text,buf,size);

    if (offset)
    {
	fprintf(stderr,"FIXME: ixp_union_ops_write() offset=%d > 0 - not supported. sending EPERM\n", offset);
	return -EPERM;
    }
    
    switch (ent->type)
    {
	case P9_PL_INT:
	    return ((ixp_union_write_long(def,ent,text)) ? size : 0);
	case P9_PL_STRING:
	    return ((ixp_union_write_string(def,ent,text)) ? size : 0);
	default:
	    fprintf(stderr,"ixp_union_ops_write() unsupported ent type %d - sending EPERM\n", ent->type);
	    return -EPERM;
    }
}

// FIMXE: add sanity checks
static long ixp_union_ops_read(MIXPSRV_FILE_HANDLE* f, long offset, long size, void* buf)
{
    Ixp_Union_Def * def = (Ixp_Union_Def*) f->priv.name;
    Ixp_Union_Ent * ent = (Ixp_Union_Ent*) f->priv.text;

    fprintf(stderr,"ixp_union_ops_read() reading from \"%s\" offset %ld size %ld\n", ent->name, offset, size);

    // the buffer is *always* zero'ed first. our clients can rely on that
    memset(buf,0,size);

    switch (ent->type)
    {
	case P9_PL_STRING:
	    // we've got an constant value
	    if (ent->value)
	    {
		// FIXME: should handle offsets !
		if (offset)
		{
		    fprintf(stderr,"read_string() does not yet support offsets in consts (%d)\n", offset);
		    return 0;
		}
		strncpy(buf,ent->value,size-1);
		return strlen(buf);
	    }

	    // no constant value -- check for getString() handler
	    if (def->ops.getString == NULL)
	    {
		fprintf(stderr,"read_string() no getString handler in this propertylist\n");
		strcpy(buf,"");
		return 0;
	    }

	    // offsets are not yet support :(
	    if (offset)
	    {
		fprintf(stderr,"read_string() does not support offsets (%d)\n", offset);
		return 0;
	    }

	    // evrything seems okay - call the getString() handler
	    fprintf(stderr,"reading string\n");
	    def->ops.getString(def,ent,buf,size);
	    return strlen(buf);
	break;

	case P9_PL_INT:
	    // check if we have an constant value
	    if (ent->value)
	    {
		strncpy(buf,ent->value,size-1);
		return strlen(buf);
	    }

	    if (offset)
	    {
		fprintf(stderr,"read_int() does not support offsets (%d)\n", offset);
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
	break;

	case P9_PL_DIR:
	    fprintf(stderr,"Reading text from directories not supported yet\n");
	    return 0;
	break;

	default:
	    fprintf(stderr,"ixp_union_ops_read() unsupported entry type %d\n", ent->type);
	    return 0;
    }

    return 0;
}

static long ixp_union_ops_size(MIXPSRV_FILE_HANDLE* f)
{
    char buffer[1024];
    buffer[0] = 0;
    ixp_union_ops_read(f, 0, sizeof(buffer)-1, buffer);
    return strlen(buffer);
}

static int ixp_union_ops_open(MIXPSRV_FILE_HANDLE* f, long mode)
{
    Ixp_Union_Def * def = (Ixp_Union_Def*) f->priv.name;
    Ixp_Union_Ent * ent = (Ixp_Union_Ent*) f->priv.text;

//    fprintf(stderr,"ixp_union_ops_open() mode %ld\n", mode);
//    fprintf(stderr,"ixp_union_ops_open() \"%s\" mode %ld\n", ent->name, mode);
    return 0;
}

static int ixp_union_ops_stat (MIXPSRV_FILE_HANDLE* f, MIXP_STAT* stat)
{
    Ixp_Union_Def * def = (Ixp_Union_Def*) f->priv.name;

    mixpsrv_default_ops_stat(f,stat);
    if (def->uid)
	stat->uid = strdup(def->uid);

    if (def->gid)
	stat->gid = strdup(def->gid);

    return 1;
}


static int ixp_union_ops_stat (MIXPSRV_FILE_HANDLE* f, MIXP_STAT* stat)
{
    Ixp_Union_Def * def = (Ixp_Union_Def*) f->priv.name;
    Ixp_Union_Ent * ent = (Ixp_Union_Ent*) f->priv.text;

    mixpsrv_default_ops_stat(f,stat);
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
