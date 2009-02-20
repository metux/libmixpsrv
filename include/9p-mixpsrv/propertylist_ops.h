
#ifndef __IXPSERV_PROPERTYLIST_OPS_H
#define __IXPSERV_PROPERTYLIST_OPS_H

#include <9p-mixp/mixp.h>
#include <9p-mixpsrv/types.h>
#include <malloc.h>
#include <memory.h>

typedef struct _MIXP_PROPERTYLIST_DEF MIXP_PROPERTYLIST_DEF;
typedef struct _MIXP_PROPERTYLIST_ENT MIXP_PROPERTYLIST_ENT;

typedef enum
{
    P9_PL_STRING    = 1,
    P9_PL_INT       = 2,
    P9_PL_DIR       = 3,
    P9_PL_FILE      = 4,
    P9_PL_INT_VAL   = 5,		// just like INT, but the integer value is stored as long
    P9_PL_FLOAT_VAL = 6,
    P9_PL_LONG_PTR  = 7,		// pointer to an long value
} MIXP_PROPERTYLIST_ENT_TYPE;

#define MIXP_PROPERTYLIST_ENTRY_ATTR_AUTOFREE	1

struct _MIXP_PROPERTYLIST_ENT
{
    int                        id;		// ID (for user only)
    const char*                name;		// NULL means: last entry in list
    const char*                value;		// NULL means: ask the getValue() proc
    long long                  int_value;	// for PL_INTVAL:   first try to ask getValue(), if none take this
    float	               float_value;	// for PL_FLOATVAL: first try to ask getValue(), if none take this
    MIXP_PROPERTYLIST_ENT_TYPE type;
    void*                      priv;		// private data
    const char*                uid;		// User-ID (NULL to use default)
    const char*                gid;		// Group-ID (NULL to use default)
    MIXPSRV_FILE_HANDLE* (*getFileHandle)(MIXP_PROPERTYLIST_DEF* def, MIXP_PROPERTYLIST_ENT* ent);
};

typedef struct
{
    int         (*free)     (MIXP_PROPERTYLIST_DEF* def);
    int         (*getInt)   (MIXP_PROPERTYLIST_DEF* def, MIXP_PROPERTYLIST_ENT* ent, long* ret);
    int         (*setInt)   (MIXP_PROPERTYLIST_DEF* def, MIXP_PROPERTYLIST_ENT* ent, long value);
    int         (*setString)(MIXP_PROPERTYLIST_DEF* def, MIXP_PROPERTYLIST_ENT* ent, const char* value);
    int         (*getString)(MIXP_PROPERTYLIST_DEF* def, MIXP_PROPERTYLIST_ENT* ent, char* buffer, long max);
    MIXPSRV_FILE_HANDLE* (*getChild) (MIXP_PROPERTYLIST_DEF* def, MIXP_PROPERTYLIST_ENT* ent);
} MIXP_PROPERTYLIST_OPS;

struct _MIXP_PROPERTYLIST_DEF
{
    MIXP_PROPERTYLIST_ENT* entries;
    MIXP_PROPERTYLIST_OPS  ops;
    void*                 priv;
    const char*           uid;
    const char*           gid;
};

/* -- file access to an in-memory string (read-only) -- */
MIXPSRV_FILE_HANDLE* mixp_propertylist_create(const char* str, MIXP_PROPERTYLIST_DEF* def);

MIXP_PROPERTYLIST_DEF* mixp_propertylist_create_def(
    MIXP_PROPERTYLIST_ENT* entries, 
    MIXP_PROPERTYLIST_OPS ops,
    const char* uid,
    const char* gid,
    void* private
);

// declare an P9_PL_INT with ID but no value
#define _MIXP_PL_DECL_INT_ID(n,i)	\
    {					\
	.name	= n,			\
	.type	= P9_PL_INT,		\
	.id	= i			\
    }

// declare an P9_PL_LONG_PTR with ID but no value
#define _MIXP_PL_DECL_LONG_PTR(n,v)	\
    {					\
	.name	= n,			\
	.type	= P9_PL_LONG_PTR,	\
	.value	= (char*)v		\
    }

// declare an P9_PL_STRING with ID but no value
#define _MIXP_PL_DECL_STRING_ID(n,i)	\
    {					\
	.name	= n,			\
	.type	= P9_PL_STRING,		\
	.id	= i			\
    }

// declare an P9_PL_STRING with constant value (no ID)
#define _MIXP_PL_DECL_STRING_CONST(n,v)	\
    {					\
	.name	= n,			\
	.type	= P9_PL_STRING,		\
	.value	= v			\
    }

// declare an P9_PL_DIR with an getFileHandle() proc
#define _MIXP_PL_DECL_DIR_PROC(n,p)	\
    {					\
	.name		= n,		\
	.type		= P9_PL_DIR,	\
	.getFileHandle	= p		\
    }

// declare an end-of-list
#define _MIXP_PL_DECL_END		\
    {					\
	.name		= NULL		\
    }

#endif
