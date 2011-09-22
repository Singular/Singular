LIB "tst.lib"; tst_init();
LIB "toric.lib";
ring r=0,(x,y,z),dp;
// call with two arguments
intmat A[2][3]=1,1,0,0,1,1;
A;
ideal I=toric_ideal(A,"du");
I;
I=toric_ideal(A,"blr");
I;
// call with three arguments
intvec prsv=1,2,1;
I=toric_ideal(A,"blr",prsv);
I;
tst_status(1);$
