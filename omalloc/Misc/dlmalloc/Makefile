CC = cc
CCC = CC
CFLAGS = -O2
RM = rm -f
DISTFILES = COPYRIGHT Makefile malloc-trace.c malloc-trace.h \
            print-trace.c trace-test.c 

.c.o:
	$(CC) $(CFLAGS) -c $<

.cc.o:
	$(CCC) $(CFLAGS) -c $<

all:
	@echo "Type make <os> to create a shared object malloc-trace.so."
	@echo "<os> can be \`linux' or \`sgi' currently."

malloc-trace.o: malloc.c

malloc.c:
	@echo "Please create a symbolic link from the malloc source"
	@echo "that you want to use to 'malloc.c'."
	@exit 1

linux: malloc.c
	$(CC) -Wall $(CFLAGS) -fpic -shared -o malloc-trace.so malloc-trace.c

sgi: malloc-trace.o
	$(CC) -shared -o malloc-trace.so malloc-trace.o -lc
	$(RM) so_locations

trace-test:
	$(CC) $(CFLAGS) trace-test.c -o trace-test

print-trace:
	$(CC) $(CFLAGS) print-trace.c -o print-trace

dist:
	tar cf - $(DISTFILES) | gzip -9 > ../mtrace.tgz
