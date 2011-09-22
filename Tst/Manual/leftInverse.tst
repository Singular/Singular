LIB "tst.lib"; tst_init();
LIB "control.lib";
// a trivial example:
ring r = 0,(x,z),dp;
matrix M[2][1] = 1,x2z;
print(M);
print( leftInverse(M) );
kill r;
// derived from the example TwoPendula:
ring r=(0,m1,m2,M,g,L1,L2),Dt,dp;
matrix U[3][1];
U[1,1]=(-L2)*Dt^4+(g)*Dt^2;
U[2,1]=(-L1)*Dt^4+(g)*Dt^2;
U[3,1]=(L1*L2)*Dt^4+(-g*L1-g*L2)*Dt^2+(g^2);
module M = module(U);
module L = leftInverse(M);
print(L);
// check
print(L*M);
tst_status(1);$
