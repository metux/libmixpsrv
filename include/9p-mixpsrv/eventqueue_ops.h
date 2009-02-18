
#ifndef __MIXPSRV_EVENTQUEUE_H
#define __MIXPSRV_EVENTQUEUE_H

#include <malloc.h>
#include <pthread.h>

typedef struct _mixpsrv_eventqueue	MIXPSRV_EVENTQUEUE;
typedef struct _mixpsrv_eventrec	MIXPSRV_EVENTREC;

struct _mixpsrv_eventqueue
{
    MIXPSRV_EVENTREC* start;
    MIXPSRV_EVENTREC* end;
    pthread_cond_t    waitq;
    pthread_mutex_t   waitq_mutex;
    pthread_mutex_t   write_lock;
    int               seq;
};

struct _mixpsrv_eventrec
{
    char*             event;
    MIXPSRV_EVENTREC* next;
    int               ref;
    int	              id;
};

static inline MIXPSRV_EVENTREC* mixpsrv_eventrec_alloc(char* content)
{
    MIXPSRV_EVENTREC* rec = (MIXPSRV_EVENTREC*)calloc(1,sizeof(MIXPSRV_EVENTREC));
    rec->event = content;
    return rec;
}

MIXPSRV_EVENTQUEUE* mixpsrv_eventqueue_alloc();
void mixpsrv_eventqueue_insert(MIXPSRV_EVENTQUEUE* queue, const char* text);
int  mixpsrv_eventfile_ops_open(MIXPSRV_FILE_HANDLE* h, long mode);
long mixpsrv_eventfile_ops_read(MIXPSRV_FILE_HANDLE* h, long offset, size_t size, void* buf);
long mixpsrv_eventfile_ops_write(MIXPSRV_FILE_HANDLE* h, long offset, size_t size, void* buf);

MIXPSRV_FILE_HANDLE* mixpsrv_eventfile_create(const char* name, MIXPSRV_EVENTQUEUE* q);

#endif
