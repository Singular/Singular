LIB "tst.lib";
tst_init();
ring r=0,(u,v,x,y,t,Dx,Dy,Dt),(a(1,1),dp);
int N=8;
matrix W[N][N]=0;
W[3,6]=1;
W[4,7]=1;
W[5,8]=1;
ncalgebra(1,W);
ideal i = t*u-x^4-y^5-x*y^4,4*x^3*v*Dt+y^4*v*Dt+Dx,5*y^4*v*Dt+4*x*y^3*v*Dt+Dy;
option(prot);
ideal I=std(i);
I;
tst_status(1);$
