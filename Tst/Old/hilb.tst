ring r1=32003,x,ds;
module m=gen(1),gen(5);
dim(m);
mult(m);
degree(m);
vdim(m);
kbase(m);
hilb(m);
m=m,x2*gen(2),x4*gen(2),x3*gen(3),x*gen(4);
dim(m);
mult(m);
degree(m);
vdim(m);
kbase(m);
hilb(m);
kill r1;

ring r2=32003,(x,y,z,w),dp;
module m0=
[x2y+yz2,0],
[yzw+w3,0],
[xyzw-1,0],
[0,x3+yzw],
[0,y3+xzw],
[0,w2+z2];
module m=std(m0);
dim(m);
mult(m);
degree(m);
hilb(m);
ideal i=maxideal(5);
hilb(i);
kbase(i);
vdim(i);
kill r2;

int n=2;
ring r3=32003,(x(1..16)),dp;
ideal i0=
x(1)+x(2)+x(3)+x(4),
x(1)^2*x(2)^2+x(2)*x(3)*x(5)*x(6),
x(3)*x(4)*x(9)*x(10)+x(4)*x(5)*x(13)*x(14),
x(1)*x(2)^2*x(3)+x(2)*x(3)*x(6)*x(7),
x(3)*x(4)*x(11)*x(12)+x(4)*x(5)*x(15)*x(16),
x(7)*x(8)*x(9)*x(10)+x(8)*x(9)*x(13)*x(14),
x(7)*x(8)*x(11)*x(12)+x(8)*x(9)*x(15)*x(16),
x(1)*x(2)*x(9)*x(10)+x(5)*x(6)*x(10)*x(11),
x(2)*x(3)*x(9)*x(10)+x(6)*x(7)*x(10)*x(11),
x(11)*x(12)*x(15)*x(16)+x(1)*x(15)*x(16)^2,
x(4)*x(5)*x(13)*x(14)+x(8)*x(9)*x(14)*x(15)+x(12)*x(13)*x(15)*x(16)+
x(1)^2*x(16)^2,
x(1)^n,x(2)^n,x(3)^n,x(4)^n,x(5)^n,x(6)^n,x(7)^n,x(8)^n,
x(9)^n,x(10)^n,x(11)^n,x(12)^n,x(13)^n,x(14)^n,x(15)^n,x(16)^n;
ideal k0=std(i0);
dim(k0);
degree(k0);
hilb(k0);
mult(k0);
vdim(k0);
kill r3;

ring r4=32003,(x(1..6)),ds;
ideal i0=
x(1)+x(2)+x(3)+x(4),
x(1)^2*x(2)^2+x(2)*x(3)*x(5)*x(6),
x(1)*x(2)^2*x(3)+x(2)*x(3)*x(6)^2;
option(returnSB); module m=syz(i0);
dim(m);
mult(m);
degree(m);
vdim(m);
hilb(m);
LIB "tst.lib";tst_status(1);$
