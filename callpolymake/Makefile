PM_INC := $(shell polymake-config --includes)
PM_CFLAGS := -fPIC -DPOLYMAKE_DEBUG=0
PM_LIBS := $(shell polymake-config --libs)
PM_LDFLAGS := $(shell polymake-config --ldflags)

CFLAGS += -march=native -DOM_NDEBUG -DNDEBUG
LDFLAGS += -Wl,-rpath,/usr/local/include/polymake/ -lxml2

all: polymake

polymake: 
	g++ -o polymake.o -I. ${PM_INC} ${PM_CFLAGS} ${CFLAGS} ${LDFLAGS} -I../ -c -g polymake.cc
	g++ -shared -o polymake.so ${PM_LDFLAGS} ${LDFLAGS} -g polymake.o ${PM_LIBS} -lgmpxx -lgmp 
	cp polymake.so ../Singular/polymake.so

clean:
	rm *.so *.o
	rm ../Singular/polymake.so

