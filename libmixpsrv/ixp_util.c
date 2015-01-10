
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

#include <9p-mixp/mixp.h>
#include <9p-mixp/fcall.h>
#include <9p-mixp/request.h>
#include <9p-mixp/fid.h>
#include <9p-mixpsrv/types.h>
#include <9p-mixpsrv/prototypes.h>
#include <9p-mixpsrv/errors.h>

/* 

This file contains code that maybe should belong into libmixp

*/

/* This should be moved to libixp */
void mixp_write_buf(MIXP_REQUEST *r, void *buf, unsigned int len)
{
	if(r->ifcall->Twrite.offset >= len)
		return;

	len -= r->ifcall->Twrite.offset;
	if(len > r->ifcall->Twrite.count)
		len = r->ifcall->Twrite.count;
	r->ofcall->Rwrite.data = calloc(1,len);
	memcpy(r->ofcall->Rwrite.data, buf + r->ifcall->Twrite.offset, len);
	r->ofcall->Rwrite.count = len;
}

/* This should be moved to libixp */
void mixp_write_to_buf(MIXP_REQUEST *r, void *buf, unsigned int *len, unsigned int max)
{
	unsigned int offset, count;

	offset = (r->fid->omode&P9_OAPPEND) ? *len : r->ifcall->Twrite.offset;
	if(offset > *len || r->ifcall->Twrite.count == 0) {
		r->ofcall->Rwrite.count = 0;
		return;
	}

	count = r->ifcall->Twrite.count;
	if(max && (count > max - offset))
		count = max - offset;

	*len = offset + count;
	
	if(max == 0) {
		*(void **)buf = ixp_erealloc(*(void **)buf, *len + 1);
		buf = *(void **)buf;
	}
		
	memcpy(buf + offset, r->ifcall->Rwrite.data, count);
	r->ofcall->Rwrite.count = count;
	((char *)buf)[offset+count] = '\0';
}

/* This should be moved to libixp */
void mixp_data_to_cstring(MIXP_REQUEST *r)
{
	unsigned int i;
	i = r->ifcall->Twrite.count;
	if(!i || r->ifcall->Twrite.data[i - 1] != '\n')
		r->ifcall->Twrite.data = ixp_erealloc(r->ifcall->Twrite.data, ++i);
	assert(r->ifcall->Twrite.data);
	r->ifcall->Twrite.data[i - 1] = '\0';
}
