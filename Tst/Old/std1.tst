//
// test script for std command
//
ring r= 32003,(x,y,z),ds;
int a = 3;
int b = 5;
int c = 2;
int d = 7;  // c*b+d*a > 2ab, ggt(a,b) = 1

poly f = (x^a+y^b)^2+x^c*y^d;
poly f'x = diff(f,x);
f;
f'x;
poly f'y = diff(f,y);
f'y;
//ideal i = f'x,f'y;
ideal i = diff(f,x), diff(f,y);
i;
"vdim         i:", vdim(i);
"dim          i:", dim(i);
"multiplicity i:", mult(i);
i;
option(prot);
test(15);
ideal j = std(i);
//degree(j);
"dim",dim(j);
"multiplicity", mult(j);
j;
//
"-------------------------------";
//
ring r1 = 32003,(x,y,z),ds;
r1;
ideal i1=maxideal(3);
ideal i2=x6,y4,z5,xy,yz,xz;
i2;
ideal j1=std(i2);
j1;
//maxdeg(3);
vdim(j1);
"---------------------------------";
i1;
//maxdeg(2);
ideal j2=std(i1);
j2;
vdim(j2);
"---------------------------------";
vector v1=[x4,y2,xz2];
vector v2=[y3,x8,2z4];
module m=v1,v2;
m;
//maxdeg(3);
module m1=std(m);
m1;
vdim(m1);
"--------------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
