LIB "tst.lib"; tst_init();
LIB "deform.lib";
ring r=0,(A,B,x,y),dp;
module M      = [x2,xy],[xy,y3],[y2],[0,x];
module kbaseM = [1],[x],[xy],[y],[0,1],[0,y],[0,y2];
poly f=xy;
module N = [AB,BBy],[A3xy+x4,AB*(1+y2)];
matrix A = lift_rel_kb(N,M,kbaseM,f);
print(A);
"TEST:";
print(matrix(kbaseM)*A-matrix(reduce(N,std(M))));
tst_status(1);$
