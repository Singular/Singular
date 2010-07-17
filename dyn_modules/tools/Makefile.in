#
#
#
CFLAGS	+= -g -I../include -I../../Singular -DNDEBUG -DMODULE_GENERATOR
LIBS	+= -L/usr/local/lib -lgmp

OBJS	= scanner.o misc.o

#	iparith.o febase.o \
#	mmalloc.o mmallocb.o mminit.o mmutil.o mmblock.o mmspec.o

# subexpr.o 

all	= modgen

.cc.o:
	gcc -c ${CFLAGS} $<

.c.o:
	gcc -c ${CFLAGS} $<

modgen: ${OBJS} modgen.h decl.inc typmap.h
	gcc -o $@ ${CFLAGS} ${OBJS} ${LIBS}

scanner.cc: scanner.l
	flex -oscanner.cc scanner.l

#	flex -Pyymod -oscanner.cc scanner.l

clean:
	rm -f a.out *~ scanner.cc *.o core

decl.inc: ../../Singular/grammar.h ../../Singular/tok.h
	-echo "/* declaration for modgen.cc */" > $@
	-grep "^#define" ../../Singular/grammar.h | awk \
		'{print "\tcase "$$2": strcpy(name,\""$$2"\"); break;"}' >> $@
	-echo "" >> $@
	-echo "/* declaration from tok.h */" >> $@
	-grep "^  [A-Z_]*," ../../Singular/tok.h | sed "s/,//g" | awk \
		'{print "\tcase "$$1": strcpy(name,\""$$1"\"); break;"}'>>$@

scanner.l:	modgen.h
scanner.cc:	scanner.l

