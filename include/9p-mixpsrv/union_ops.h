
#ifndef __IXPSERV_UNION_OPS_H
#define __IXPSERV_UNION_OPS_H

#include <9p-mixp/mixp.h>
#include <9p-mixpsrv/types.h>
#include <malloc.h>
#include <memory.h>

typedef struct _IXP_UNION_DEF Ixp_Union_Def;
typedef struct _IXP_UNION_ENT Ixp_Union_Ent;

typedef enum
{
    P9_PL_STRING = 1,
    P9_PL_INT    = 2,
    P9_PL_DIR    = 3,
    P9_PL_FILE   = 4
} IXP_UNION_ENT_TYPE;
    
struct _IXP_UNION_ENT
{
    const char*        name;
    IXP_UNION_ENT_TYPE type;

    MIXPSRV_FILE_HANDLE*		handle;
    Ixp_Union_Ent*	next;
    Ixp_Union_Ent*	childs;
};

struct _Ixp_Union_Def
{
    Ixp_Union_Ent* entries;
    Ixp_Union_Ops  ops;
    void*                 priv;
    const char*           uid;
    const char*           gid;
};

/* -- file access to an in-memory string (read-only) -- */
MIXPSRV_FILE_HANDLE* ixp_union_create(const char* str, Ixp_Union_Def* def);

static inline Ixp_Union_Def* ixp_union_create_def(
    Ixp_Union_Ent* entries, 
    Ixp_Union_Ops ops,
    const char* uid,
    const char* gid,
    void* private
)
{
    Ixp_Union_Def* def = (Ixp_Union_Def*)malloc(sizeof(Ixp_Union_Def));
    memset(def,0,sizeof(Ixp_Union_Def));
    def->entries = entries;
    def->ops     = ops;
    def->priv    = private;
    def->uid     = strdup(uid);
    def->gid     = strdup(gid);
    return def;
}

#endif
