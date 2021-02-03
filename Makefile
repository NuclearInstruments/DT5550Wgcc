COMMON_SRC = ftdi_ft60x.cpp ni_usb3_class.cpp ni_usb3_wrapper.cpp
CFLAGS     = -Ilinux-x86_64
LFLAGS     = -Llinux-x86_64
LIBS       = -l:libftd3xx.so

TARGETS    = test
all: $(TARGETS)

$(TARGETS):
	g++ -o $@ $(CFLAGS) $(LFLAGS) $@.cpp $(COMMON_SRC) $(LIBS)

clean:
	-rm -rf $(TARGETS)