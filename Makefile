#
# Author(s): Enrico Weigelt <weigelt@metux.de>
#

LIBNAME=libmixpsrv
INCNAME=9p-mixpsrv
APPNAME=ixpserv

all:	lib	$(LIBNAME).pc	app

include ./build.mk

lib:
	make -C $(LIBNAME)

app:
	make -C $(APPNAME)

install-lib:	
	make -C $(LIBNAME) install

install-pkgconfig:	$(LIBNAME).pc
	mkdir -p $(DESTDIR)$(PKGCONFIGDIR)
	cp $(LIBNAME).pc $(DESTDIR)$(PKGCONFIGDIR)
	
install-includes:	include/$(INCNAME)/*.h
	mkdir -p $(DESTDIR)$(INCLUDEDIR)/$(INCNAME)
	for i in include/$(INCNAME)/*.h ; do cp $$i $(DESTDIR)$(INCLUDEDIR)/$(INCNAME) ; done

install:	install-pkgconfig install-includes install-lib

clean:	
	rm -f *.o *.a *.so $(LIBNAME).pc
	make -C $(LIBNAME) clean
	make -C $(APPNAME) clean

