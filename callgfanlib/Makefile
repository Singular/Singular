all: gfanlib

gfanlib: 
	g++ -o bbcone.o -fpic -I/home/ren/Sources/ -c -g bbcone.cc
	g++ -o bbfan.o -fpic -I/home/ren/Sources/ -c -g bbfan.cc
	g++ -o bbpolytope.o -fpic -I/home/ren/Sources/ -c -g bbpolytope.cc
	g++ -o gfanlib.o -fpic -I/home/ren/Sources/ -c -g gfanlib.cc
	g++ -shared -o gfanlib.so bbcone.o bbfan.o bbpolytope.o gfanlib.o -L/usr/local/lib -lcddgmp -lgmpxx ../gfanlib/*.o

clean:
	rm *.so *.o

