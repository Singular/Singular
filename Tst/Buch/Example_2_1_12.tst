LIB "tst.lib";
tst_init();

ring A=0,(x,y,z),(c,dp);
matrix M[2][3]=x,xy,z,x2,xyz,yz;
print(M);

module Ker=syz(M);
Ker;

module Im=M[1],M[2],M[3];
Im;

tst_status(1);$
