LIB "tst.lib"; tst_init();
LIB "control.lib";
// a trivial example:
ring r = 0,(x,z),dp;
matrix M[1][2] = 1,x2+z;
print(M);
print( rightInverse(M) );
kill r;
// derived from the TwoPendula example:
ring r=(0,m1,m2,M,g,L1,L2),Dt,dp;
matrix U[1][3];
U[1,1]=(-L2)*Dt^4+(g)*Dt^2;
U[1,2]=(-L1)*Dt^4+(g)*Dt^2;
U[1,3]=(L1*L2)*Dt^4+(-g*L1-g*L2)*Dt^2+(g^2);
module M = module(U);
module L = rightInverse(M);
print(L);
// check
print(M*L);
tst_status(1);$
