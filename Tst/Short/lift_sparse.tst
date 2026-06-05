LIB "tst.lib";
tst_init();

ring R = 0,(x,y,z),dp;
poly f = x3+y7+z2+xyz;
ideal i = jacob(f);
module T;
module L = lift_sparse(i,i);
typeof(L);
print(L);
size(module(matrix(L)-lift(i,i)))==0;

ideal sm = liftstd_sparse(i,T);
typeof(T);
typeof(sm);
size(module(matrix(sm)-matrix(i)*matrix(T)))==0;

module S;
sm = liftstd_sparse(i,T,S,"std");
size(module(matrix(sm)-matrix(i)*matrix(T)))==0;
size(module(matrix(i)*matrix(S)))==0;
print(T);
print(S);

ring R2 = 0,(x,y,z),lp;
matrix A[2][3] = x,x,y,y,z,z;
module A1 = module(A);
module A2 = A1[3];
A1 = A1[1..2];
module T2, S2;
module G = liftstd_sparse(A1,T2,S2,"std",A2);
typeof(G);
size(reduce(module(matrix(G)-matrix(A1)*matrix(T2)),A2))==0;
size(module(matrix(A1)*matrix(S2)))==0;

module G0 = module(matrix(A1)*matrix(T2));
module L2 = lift_sparse(A1,G0);
size(module(matrix(L2)-lift(A1,G0)))==0;

tst_status(1);$
