//=== f:=f(11,10,3,1)//i:=fx,fy,fz

ring r3=32003,(x,y,z),ds;
int a =11;
int b =10;
int c =3;
int t =1;
poly f =x^a+y^b+z^(3*c)+x^(c+2)*y^(c-1)+x^(c-1)*y^(c-1)*z3+x^(c-2)*y^c*(y2+t*x)^2;
ideal i= jacob(f);
ideal i0=std( i);
size(i0);
degree(i0);
option(fastHC);
i0=std( i);
size(i0);
degree(i0);
option(nofastHC,returnSB);
i0=std( i);
size(i0);
degree(i0);
option(morePairs);
i0=std( i);
size(i0);
degree(i0);
option(notSugar);
i0=std( i);
size(i0);
degree(i0);
option(sugarCrit);
i0=std( i);
size(i0);
degree(i0);
option(nosugarCrit,nomorePairs);
i0=std( i);
size(i0);
degree(i0);
i0;
LIB "tst.lib";tst_status(1);$
