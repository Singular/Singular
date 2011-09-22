LIB "tst.lib"; tst_init();
LIB "decodegb.lib";
// Newton identities for a binary 3-error-correcting cyclic code of
//length 31 with defining set (1,5,7)
int n=31;          // length
list defset=1,5,7; //defining set
int t=3;           // number of errors
int q=2;           // basefield size
int m=5;           // degree extension of the splitting field
int tr=1;          // indicator of triangular form of Newton identities
def A=sysNewton(n,defset,t,q,m);
setring A;
A;                 // shows the ring we are working in
print(newton);     // generalized Newton identities
//===============================
A=sysNewton(n,defset,t,q,m,tr);
setring A;
print(newton);     // generalized Newton identities in triangular form
tst_status(1);$
