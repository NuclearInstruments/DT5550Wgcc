COMMON_SRC = src/circular_buffer.c src/testIpnewcore5550_lib.c 
CFLAGS     = -Ilinux-x86_64
LFLAGS     = -Llinux-x86_64 
LIBS       = -l:libftd3xx.so


all: testIpnewcore5550_example #$(TARGETS)

testIpnewcore5550_example:
	gcc -o gcc/$@ src/$@.c $(COMMON_SRC) -l:libni_usb3.so -L.

#$(TARGETS):
#	g++ -o $@ $(CFLAGS) $(LFLAGS) $@.cpp $(COMMON_SRC) $(LIBS)

clean:
	-rm -rf gcc/*.*

