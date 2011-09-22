LIB "tst.lib"; tst_init();
LIB "crypto.lib";
ring R = 0,z,dp;
number N=11;
number a=1;
number b=6;
list P,Q;
P[1]=2;
P[2]=4;
P[3]=1;
Q[1]=3;
Q[2]=5;
Q[3]=1;
ellipticAdd(N,a,b,P,Q);
tst_status(1);$
