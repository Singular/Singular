LIB "tst.lib";
tst_init();
ring r=0,(x,y,z),dp;
matrix d[3][3];
d[1,2]=-z;
d[1,3]=2x;
d[2,3]=-2y;
ncalgebra(1,d); 
// it is algebra U(sl_2)
int N=3;
poly f=1;
for(int n=0;n<=N;n++)
{ 
  f=f*(z+N-2*n);
}
f;
ideal i=x^(N+1),y^(N+1),f;
option(redSB);
option(redTail);
ideal I=std(i);
I;
ideal J=system("twostd",i);
J;
tst_status(1);$
