LIB "tst.lib"; tst_init();
LIB "finvar.lib";
ring R=0,(x,y,z),dp;
matrix A[3][3]=0,1,0,-1,0,0,0,0,-1;
matrix P,S,IS=invariant_ring_random(A,1);
print(P);
print(S);
print(IS);
tst_status(1);$
