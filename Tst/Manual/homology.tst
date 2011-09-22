LIB "tst.lib"; tst_init();
LIB "homolog.lib";
ring r;
ideal id=maxideal(4);
qring qr=std(id);
module N=maxideal(3)*freemodule(2);
module M=maxideal(2)*freemodule(2);
module B=[2x,0],[x,y],[z2,y];
module A=M;
module H=homology(A,B,M,N);
H=std(H);
// dimension of homology:
dim(H);
// vector space dimension:
vdim(H);
ring s=0,x,ds;
qring qs=std(x4);
module A=[x];
module B=A;
module M=[x3];
module N=M;
homology(A,B,M,N);
tst_status(1);$
