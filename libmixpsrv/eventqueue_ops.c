
#define __USE_GNU
#define _GNU_SOURCE

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <9p-mixp/mixp.h>
#include <9p-mixpsrv/types.h>
#include <9p-mixpsrv/default_ops.h>
#include <9p-mixpsrv/call_ops.h>
#include <9p-mixpsrv/p9srv.h>
#include <9p-mixpsrv/eventqueue_ops.h>

#define DEBUG(param...);	\
    printf("[%ld] ", pthread_self());	\
    printf(#param);			\
    printf("\n");

MIXPSRV_EVENTQUEUE* mixpsrv_eventqueue_alloc()
{
    MIXPSRV_EVENTQUEUE* q = (MIXPSRV_EVENTQUEUE*)calloc(1,sizeof(MIXPSRV_EVENTQUEUE));
    q->start       = q->end = mixpsrv_eventrec_alloc(strdup("ROOT"));

    pthread_mutex_init(&(q->waitq_mutex),NULL);
    pthread_mutex_init(&(q->write_lock),NULL);
    pthread_cond_init(&(q->waitq),NULL);

    return q;
}

static inline void _queue_lock(MIXPSRV_EVENTQUEUE* queue)
{
//    DEBUG("LOCK  queue->write_lock");
    pthread_mutex_lock(&(queue->write_lock));
//    DEBUG("GOT   queue->write_lock");
}

static inline void _queue_unlock(MIXPSRV_EVENTQUEUE* queue)
{
//    DEBUG("UNLOCK write_lock");
    pthread_mutex_unlock(&(queue->write_lock));
//    DEBUG("LEFT   write_lock");
}

static void _purge(MIXPSRV_EVENTQUEUE* queue)
{
    _queue_lock(queue);
    DEBUG("purging queue ...");
    int running = 1;
    while (running)
    {
	if (queue->start->next == NULL)
	{
	    DEBUG("EOQ");
	    break;
	}
	else if (queue->start->next->ref==0)
	{
	    MIXPSRV_EVENTREC* walk = queue->start->next;
	    printf("Releasing unused: (%d) %s\n", walk->id, walk->event);
	    queue->start->next = walk->next;
	    if (walk->event)
		free(walk->event);
	    free(walk);
	    break;
	}
	else
	{
	    printf("Got someting used: (%d) %d %s\n",
		queue->start->next->id,
		queue->start->next->ref,
		queue->start->next->event);
	    break;
	}
    }
    _queue_unlock(queue);
}

void mixpsrv_eventqueue_insert(MIXPSRV_EVENTQUEUE* q, const char* text)
{
    MIXPSRV_EVENTREC* rec = mixpsrv_eventrec_alloc(strdup(text));
    _queue_lock(q);
    rec->id = q->seq++;
    q->end->next = rec;
    q->end = rec;
    _queue_unlock(q);
}

int mixpsrv_eventfile_ops_open(MIXPSRV_FILE_HANDLE* fp, long mode)
{
    fprintf(stderr,"mixpsrv_eventfile_ops_open() mode=%ld\n", mode);
    return 0;
}

static void _dump(MIXPSRV_EVENTQUEUE* queue)
{
    _queue_lock(queue);
    DEBUG("Dumping queue:");
    MIXPSRV_EVENTREC* walk = queue->start;
    while (walk)
    {
	printf("[id %d] ref=%d value=\"%s\"\n", walk->id, walk->ref, walk->event);
	walk = walk->next;
    }
    _queue_unlock(queue);
}

long mixpsrv_eventfile_ops_read(MIXPSRV_FILE_HANDLE* fp, long offset, size_t size, void* buf)
{
    printf("eventfile::read() offset=%ld size=%d\n", offset, size);
    MIXPSRV_EVENTQUEUE* queue = (MIXPSRV_EVENTQUEUE*)fp->priv.text;

    while (1)
    {
	_dump(queue);
	_purge(queue);
	MIXPSRV_EVENTREC* ent   = (MIXPSRV_EVENTREC*)fp->priv.ptr;
	MIXPSRV_EVENTREC* next  = ent->next;
        if (next)
	{
	    printf("readA: ent->  id=%d ref=%d val=%s\n", ent->id, ent->ref, ent->event);
	    printf("readA: next-> id=%d ref=%d val=%s\n", next->id, next->ref, ent->event);
	    _queue_lock(queue);
	    next->ref++;
	    ent->ref--;
	    _queue_unlock(queue);
	    printf("readB: ent->  id=%d ref=%d val=%s\n", ent->id, ent->ref, ent->event);
	    printf("readB: next-> id=%d ref=%d val=%s\n", next->id, next->ref, ent->event);
	    strncpy(buf, next->event, size);
	    fp->priv.ptr = next;
	    return strlen(buf);
        }
	else
	    sleep(1);
    }
}

long mixpsrv_eventfile_ops_write(MIXPSRV_FILE_HANDLE* fp, long offset, size_t size, void* buf)
{
    mixpsrv_eventqueue_insert((MIXPSRV_EVENTQUEUE*)fp->priv.text,strndup(buf,size));
    return size;
}

static MIXPSRV_FILE_OPS _ops =
{
    .classname	= "EVENTQUEUE",
    .open	= mixpsrv_eventfile_ops_open,
    .read	= mixpsrv_eventfile_ops_read,
    .write	= mixpsrv_eventfile_ops_write
};

MIXPSRV_FILE_HANDLE* mixpsrv_eventfile_create(const char* name, MIXPSRV_EVENTQUEUE* queue)
{
    MIXPSRV_FILE_HANDLE* fp = p9srv_get_file();
    fp->ops       = _ops;
    fp->async     = 1;
    fp->name      = strdup(name);
    fp->priv.text = (char*)queue;
    fp->qtype     = P9_QTFILE;
    fp->type      = 0;
    fp->perm      = 0555;

    // attaching new FD to the end of the queue
    _queue_lock(queue);
    fp->priv.ptr  = queue->end;
    queue->end->ref++;
    _queue_unlock(queue);

    return fp;
}
