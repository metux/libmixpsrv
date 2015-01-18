
SONAME=mixpsrv

SRCNAMES  = default_ops hostfile_ops ixp_util p9srv_ops rostring_ops propertylist_ops server
NOPIC_OBJ = $(addsuffix .nopic.o, $(SRCNAMES))
PIC_OBJ   = $(addsuffix .pic.o, $(SRCNAMES))

CFLAGS+=-I../include $(MIXP_CFLAGS)
LDFLAGS+=$(MIXP_LIBS) -lmixp -lc

all:	info lib$(SONAME).a lib$(SONAME).so

info:
	@echo "CFLAGS=$(CFLAGS)"
	@echo "LDFLAGS=$(LDFLAGS)"

include ../build.mk

lib$(SONAME).a::	$(NOPIC_OBJ)

lib$(SONAME).so::	$(PIC_OBJ)

install:	lib$(SONAME).a	lib$(SONAME).so
	@mkdir -p $(DESTDIR)$(LIBDIR)
	@cp lib$(SONAME).a lib$(SONAME).so $(DESTDIR)$(LIBDIR)
	@chmod ugo+x $(DESTDIR)$(LIBDIR)/lib$(SONAME).so

clean:
	@rm -f *.o *.a *.so
