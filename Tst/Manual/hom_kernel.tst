LIB "tst.lib"; tst_init();
LIB "homolog.lib";
ring r;
module N=[2x,x],[0,y];
module M=maxideal(1)*freemodule(2);
matrix A[2][3]=2x,0,x,y,z2,y;
module K=hom_kernel(A,M,N);
// dimension of kernel:
dim(std(K));
// vector space dimension of kernel:
vdim(std(K));
print(K);
tst_status(1);$
