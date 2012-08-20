LIB "tst.lib";
tst_init();

// wrong enterOnePairSpecial
ring R=integer,(x,y,z),dp;
ideal I=6xz+yz,xz+yz+z;
std(I);
quotient(I,5);

tst_status(1);$
