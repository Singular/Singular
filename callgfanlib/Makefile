all: gfanlib

gfanlib: 
	g++ -DOM_DEBUG -DNDEBUG -o bbcone.o -fpic -I/home/ren/Sources/ -c -g bbcone.cc
	g++ -DOM_DEBUG -DNDEBUG -o bbfan.o -fpic -I/home/ren/Sources/ -c -g bbfan.cc
	g++ -DOM_DEBUG -DNDEBUG -o bbpolytope.o -fpic -I/home/ren/Sources/ -c -g bbpolytope.cc
	g++ -DOM_DEBUG -DNDEBUG -o gfanlib.o -fpic -I/home/ren/Sources/ -c -g gfanlib.cc
	g++ -shared -o gfanlib.so bbcone.o bbfan.o bbpolytope.o gfanlib.o ../gfanlib/*.o -L/usr/local/lib -lcddgmp -lgmpxx 

clean:
	rm *.so *.o

