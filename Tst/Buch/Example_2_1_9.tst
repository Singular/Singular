LIB "tst.lib";
tst_init();

ring   A=0,(x,y,z),dp;
module M=[xy-1,z2+3,xyz],[y4,x3,z2];
M;
ideal  I=x2+y2+z2;
qring  Q=std(I);
module M=fetch(A,M);


tst_status(1);$
