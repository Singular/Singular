CFLAGS += -march=native -DOM_NDEBUG -DNDEBUG -O3 -fPIC
CXX = g++ -m32

all: dummy

dummy: 
	${CXX} -o dummy.o -I. ${CFLAGS} -I../ -c dummy.cc
	${CXX} -shared -o dummy.so ${LDFLAGS} dummy.o 
	cp dummy.so ../Singular/dummy.so

clean:
	rm *.so *.o
	rm ../Singular/dummy.so

