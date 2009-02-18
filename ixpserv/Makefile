#
# Author(s): Enrico Weigelt <weigelt@metux.de>
#

CFLAGS+=-I../include/ -L../libmixpsrv/ ../libmixpsrv/libmixpsrv.a
#MIXP_LIB=-Xlinker -R$(MIXP_PREFIX)/lib -L$(MIXP_PREFIX)/lib -lpthread

# CFLAGS += -DVERSION=\"${VERSION}\" -I../include
# LIBMIXP=../libmixp/libmixp.a -lpthread

all:		ixpserv

include ../build.mk

ixpserv:	fs.o ../libmixpsrv/libmixpsrv.a
	$(CC) -o $@ $^ $(MIXP_CFLAGS) $(MIXP_LIBS) $(CFLAGS)

clean:
	rm -f *.o ixpserv

