LIB "tst.lib";
tst_init();

ring R=0,(x,y,z),(c,dp);
ideal K=x2y2-xyz2;
qring S=std(K);
module V=[x2,xy,y2],[xy,xz,yz];
matrix M[3][2]=x,y,zx,zy,y2,z2;
module N=M[1],M[2],V[1],V[2];
module Re=syz(N);
module Ker;
int i;
for(i=1;i<=size(Re);i++){Ker=Ker+Re[i][1..2];}
Ker;

reduce(M*Ker,std(V));

tst_status(1);$
