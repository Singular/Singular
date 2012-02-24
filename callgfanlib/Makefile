all: gfanlib

gfanlib: 
	g++ -DOM_DEBUG -DNDEBUG -o bbcone.o -fpic -I../ -c -g bbcone.cc
	g++ -DOM_DEBUG -DNDEBUG -o bbfan.o -fpic -I../ -c -g bbfan.cc
	g++ -DOM_DEBUG -DNDEBUG -o bbpolytope.o -fpic -I../ -c -g bbpolytope.cc
	g++ -DOM_DEBUG -DNDEBUG -o gfanlib.o -fpic -I../ -c -g gfanlib.cc
	g++ -DOM_DEBUG -DNDEBUG -o gfan.o -fpic -I../ -c -g gfan.cc
	g++ -shared -o gfanlib.so bbcone.o bbfan.o bbpolytope.o gfanlib.o gfan.o ../gfanlib/*.o -L/usr/local/lib -lcddgmp -lgmpxx 
	cp gfanlib.so ../Singular/gfanlib.so

clean:
	rm *.so *.o
	rm ../Singular/gfanlib.so
