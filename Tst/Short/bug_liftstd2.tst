LIB "tst.lib";
tst_init();

// lifstd with redLiftstd, relative version

LIB "matrix.lib";

ring R = 0,(x,y,z),(c,dp);

matrix A[1][1] = x;
matrix B[1][2] = x+y-z,x-y-z;

matrix C[nrows(A)][ncols(A) + ncols(B)] = concat(A,B);

matrix G1 = std(C);

matrix T;
module G2 = liftstd(module(A), T, "std", module(B));

print("");
print("reduce(G1,G2):");
print(reduce(G1,G2));
print("reduce(A*T-G2,std(B)):");
print(reduce(A*T-matrix(G2),std(B)));

tst_status(1);$
