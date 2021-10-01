COMMON_SRC = ftdi_ft60x.cpp ni_usb3_class.cpp ni_usb3_wrapper.cpp
CFLAGS     = -Ilinux-x86_64
LFLAGS     = -Llinux-x86_64 
LIBS       = -l:libftd3xx.so


all: libni_usb3.so test test_dt5550 #$(TARGETS)

libni_usb3.so:
	g++ -o $@ $(CFLAGS) $(LFLAGS) $(COMMON_SRC) $(LIBS) -shared -fPIC  -g

test:
	g++ -o $@ $@.cpp -l:libni_usb3.so -L.

test_dt5550:
	g++ -o $@ $@.cpp -l:libni_usb3.so -L.
#$(TARGETS):
#	g++ -o $@ $(CFLAGS) $(LFLAGS) $@.cpp $(COMMON_SRC) $(LIBS)

clean:
	-rm -rf libni_usb3.so test test_dt5550


# PREFIX is environment variable, but if it is not set, then set default value
ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

install: libni_usb3.so
	install -d $(DESTDIR)$(PREFIX)/lib/
	install -m 644 libni_usb3.so $(DESTDIR)$(PREFIX)/lib/
	install -d $(DESTDIR)$(PREFIX)/include/
	install -m 644 ni_usb3_wrapper.h $(DESTDIR)$(PREFIX)/include/
	ldconfig 
