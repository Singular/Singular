PM_INC := $(shell polymake-config --includes)
PM_CFLAGS := -fPIC -DPOLYMAKE_DEBUG=0
PM_LIBS := $(shell polymake-config --libs)
PM_LDFLAGS := $(shell polymake-config --ldflags)

CFLAGS += -march=native
LDFLAGS += -Wl,-rpath,/home/ren/polymake-2.9.10/build.x86_64/perlx-5.10.1-x86_64-linux-gnu-thread-multi/ -lxml2

all: polymake

polymake: 
	g++ -o polymake.o -I. ${PM_INC} ${PM_CFLAGS} ${CFLAGS} ${PM_LDFLAGS} ${PM_LIBS} -I/home/ren/Sources/ -c -g polymake.cc
	g++ -shared -o polymake.so ${PM_INC} ${PM_CFLAGS} ${CFLAGS} ${PM_LDFLAGS} ${PM_LIBS} ${LDFLAGS} -lgmpxx -lgmp -g polymake.o

clean:
	rm *.so *.o

