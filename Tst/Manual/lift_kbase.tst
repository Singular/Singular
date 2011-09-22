LIB "tst.lib"; tst_init();
LIB "deform.lib";
ring R=0,(x,y),ds;
module M=[x2,xy],[y2,xy],[0,xx],[0,yy];
module N=[x3+xy,x],[x,x+y2];
print(M);
module kb=kbase(std(M));
print(kb);
print(N);
matrix A=lift_kbase(N,M);
print(A);
matrix(reduce(N,std(M)),nrows(kb),ncols(A)) - matrix(kbase(std(M)))*A;
tst_status(1);$
