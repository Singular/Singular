LIB "tst.lib"; tst_init();
LIB "linalg.lib";
ring r=0,(t,x),lp;
matrix A[2][2]=1,x2,x,x2+3x;
print(A);
matrix Adj[2][2]=adjoint(A);
print(Adj);                    //Adj*A=det(A)*E
print(Adj*A);
tst_status(1);$
