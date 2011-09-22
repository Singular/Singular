LIB "tst.lib"; tst_init();
LIB "crypto.lib";
ring R = 0,z,dp;
number N=11;
number a=1;
number b=6;
list P;
P[1]=2;
P[2]=4;
P[3]=1;
ellipticMult(N,a,b,P,3);
tst_status(1);$
