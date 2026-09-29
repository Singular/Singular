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
bigintvec hBigSmall=hilb(std(J),1,bigintvec(1,2,1));
h32==hBigSmall;
ideal G32=std(J,h32,intvec(1,2,1));
ideal Gbig=std(J,h32,bigintvec(1,2,1));
matrix(G32)==matrix(Gbig);

// Large weights are active in every generator, not merely accepted on an
// unused variable.  The corresponding small intvec and bigintvec calls agree.
bigintvec activeWeights=bigintvec(3000000000,6000000000,3000000000);
homog(J,activeWeights);
homog(J,intvec(1,2,1))==homog(J,bigintvec(1,2,1));

// Module component shifts remain intvec for compatibility, while variable
// weights may now be bigintvec.  Full 64-bit module shifts are a later step.
module M=[x2,y],[x,z];
intvec shifts=1,0;
homog(M,intvec(1,3,2),shifts)
  ==homog(M,bigintvec(1,3,2),shifts);
homog(M,bigintvec(3000000000,6000000001,3000000001),shifts);
homog(M,activeWeights,shifts);

// Check the four-argument incremental entry point as well.
poly p=y-z;
ideal G4=std(std(J),p,h32,bigintvec(1,2,1));
ideal expected=std(J+ideal(p));
matrix(std(G4))==matrix(expected);

tst_status(1);$
