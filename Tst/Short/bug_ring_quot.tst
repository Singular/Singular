LIB "tst.lib";
tst_init();

// wrong enterOnePairSpecial
ring R=integer,(x,y,z),dp;
ideal I=6xz+yz,xz+yz+z;
std(I);
quotient(I,5);

// tr. # 490
ring RR=integer,(x,y),dp;
module N=[x,1],[x,y];
module M=freemodule(2);
quotient(N,M);


tst_status(1);$
