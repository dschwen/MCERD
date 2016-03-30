CC = gcc
CFLAGS = -O2
# CFLAGS = -g -Wall -Wmissing-prototypes
LIB = -lm
LDFLAGS = # -g
PROG = zbl96

OBJS = zbl96stop.o zbl96main.o 
LIBOBJS = zbl96stop.o zbl96lib.o

$(PROG): $(OBJS)
	$(CC) $(LFLAGG) -o $(PROG) $(OBJS) $(LIB)

lib: libzbl96.a $(LIBOBJS)

clean:
	rm  -f $(OBJS) $(LIBOBJS) $(PROG) libzbl96.a test_libzbl96 testlib.o 

testlib: testlib.o $(LIBOBJS) libzbl96.a
	$(CC) $(LDFLAGS) -o test_libzbl96 testlib.o -L. -lzbl96 $(LIB)

libzbl96.a: $(LIBOBJS)
	ar r libzbl96.a $(LIBOBJS)
	ranlib libzbl96.a

install:
	mkdir -p /usr/local/bin /usr/local/data
	install -s zbl96 /usr/local/bin
	install scoef.95a scoef.95b masses.dat /usr/local/data
        
inst-lib: lib
	mkdir -p /usr/local/lib /usr/local/include
	install -s test_libzbl96 /usr/local/bin
	install libzbl96.a /usr/local/lib
	install zbl96.h /usr/local/include

inst-doc:
	mkdir -p /usr/doc/zbl96-0.99a
	install ChangeLog README TODO /usr/doc/zbl96-0.99a

depend .depend:
	$(CC) -MM *.c > .depend

include .depend
