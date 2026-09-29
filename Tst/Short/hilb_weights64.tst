LIB "tst.lib";
tst_init();

// A weight beyond signed 32 bit is accepted at the interpreter boundary.
// The unused variable has the large weight, so the public dense Hilbert
// vector remains small while exercising the bigintvec-to-int64 conversion.
ring r=32003,(x,y,z),dp;
ideal I=y,z;
ideal G=std(I);
bigintvec largeWeights=bigintvec(3000000000,1,1);
bigintvec h=hilb(G,1,largeWeights);
ideal G64=std(I,h,largeWeights);
h;
matrix(G64)==matrix(G);

// The old intvec call and the new bigintvec call remain interchangeable.
ideal J=x2-y,x-z;
bigintvec h32=hilb(std(J),1,intvec(1,2,1));
ideal G32=std(J,h32,intvec(1,2,1));
ideal Gbig=std(J,h32,bigintvec(1,2,1));
matrix(G32)==matrix(Gbig);

// Check the four-argument incremental entry point as well.
poly p=y-z;
ideal G4=std(std(J),p,h32,bigintvec(1,2,1));
ideal expected=std(J+ideal(p));
matrix(std(G4))==matrix(expected);

tst_status(1);$
