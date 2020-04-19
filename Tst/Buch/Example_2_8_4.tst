LIB "tst.lib";
tst_init();

ring R=0,(x,y,z),(c,dp);
ideal I=x2y2-xyz2;
qring A=std(I);
module V=[x2,xy,y2],[xy,xz,yz];
matrix B[3][2]=x,y,zx,zy,y2,z2;
module N=B[1],B[2],V[1],V[2];
option(returnSB);
module Re=syz(N);
option(noreturnSB);
module Ker;
int i;
for(i=1;i<=ncols(Re);i++){Ker=Ker+Re[i][1..2];}
Ker;

reduce(B*Ker,std(V));

modulo(B,V);

tst_status(1);$
