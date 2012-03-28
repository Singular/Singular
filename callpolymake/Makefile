PM_INC := $(shell polymake-config --includes)
PM_CFLAGS := -fPIC -DPOLYMAKE_DEBUG=0
PM_LIBS := $(shell polymake-config --libs)
PM_LDFLAGS := $(shell polymake-config --ldflags)

CFLAGS += -march=native -DOM_NDEBUG -DNDEBUG
LDFLAGS += -Wl,-rpath,/usr/local/include/polymake/ -lxml2

all: polymake

polymake: 
	g++ -o polymake_conversion.o -I. ${PM_INC} ${PM_CFLAGS} ${CFLAGS} ${LDFLAGS} -I../ -c -g polymake_conversion.cc
	g++ -o polymake_documentation.o -I. ${PM_INC} ${PM_CFLAGS} ${CFLAGS} ${LDFLAGS} -I../ -c -g polymake_documentation.cc
	g++ -o polymake_wrapper.o -I. ${PM_INC} ${PM_CFLAGS} ${CFLAGS} ${LDFLAGS} -I../ -c -g polymake_wrapper.cc
	g++ -shared -o polymake.so ${PM_LDFLAGS} ${LDFLAGS} -g polymake_conversion.o -g polymake_documentation.o polymake_wrapper.o ${PM_LIBS} -lgmpxx -lgmp 
	cp polymake.so ../Singular/polymake.so

clean:
	rm *.so *.o
	rm ../Singular/polymake.so

