option(prot);
ring cy=0,(a,b,c,d,e),dp;
int n=5;

ideal i=
a^n+b,b^n+c,c^n+d,d^n+e,a+b+c+e;
i=std(i);
qring q=i;
poly p2=de+ab;
poly p3=cde+abc;
poly p4=bcde+abde+abce+abcd;
poly p5=a+b+c+d;

module m1=
p2*gen(2),
p2*gen(1)+p3*gen(2),
p3*gen(1)+p4*gen(2),
p4*gen(1)+p5*gen(2),
p5*gen(1);
option(redSB);
option();
module j=std(m1);
option(noredSB);
option(noredTail);
option();
module k=std(m1);
j;
k;
int h=size(j);
h;
size(k);
int s;
for(s=1;s<=h;s=s+1)
{
  NF(k[s],j);
}
for(s=1;s<=h;s=s+1)
{
  NF(j[s],k);
}
kill cy,h,s,n;

ring s1=(0,a),(x,y,z,w),dp;
ideal i0=(a2-1)*x2-z10-z20,(a+1)*xy3-z10-z30,y6-(a+1)*xy3w40;
option();
ideal i = std(i0);
option(redTail);
option();
ideal i1 = std(i0);
i;
i1;
int h=size(i);
h;
size(i1);
int s;
for(s=1;s<=h;s=s+1)
{
  NF(i1[s],i);
}
for(s=1;s<=h;s=s+1)
{
  NF(i[s],i1);
}
kill s1,h,s;


ring s2=(0,a),(x,y,z,w),dp;
minpoly=8a4+1;
ideal j0=(a2-1)*x2-z10-z20,(a+1)*xy3-z10-z30,y6-(a+1)*xy3w40;
option(noredTail);
option();
ideal i = std(j0);
option(redTail);
option();
ideal i1 = std(j0);
i;
i1;
int h=size(i);
h;
size(i1);
int s;
for(s=1;s<=h;s=s+1)
{
  NF(i1[s],i);
}
for(s=1;s<=h;s=s+1)
{
  NF(i[s],i1);
}
kill s2,h,s;


ring s3=(32003,a),(x,y,z,w),dp;
ideal k0=(a2-1)*x2-z10-z20,(a+1)*xy3-z10-z30,y6-(a+1)*xy3w40;
option(noredTail);
option();
ideal i = std(k0);
option(redTail);
option();
ideal i1 = std(k0);
i;
i1;
int h=size(i);
h;
size(i1);
int s;
for(s=1;s<=h;s=s+1)
{
  NF(i1[s],i);
}
for(s=1;s<=h;s=s+1)
{
  NF(i[s],i1);
}
kill s3,h,s;


ring s4=(32003,a),(x,y,z,w),dp;
minpoly=8a4+1;
ideal l0=(a2-1)*x2-z10-z20,(a+1)*xy3-z10-z30,y6-(a+1)*xy3w40;
option(noredTail);
option();
ideal i = std(l0);
option(redTail);
option();
ideal i1 = std(l0);
i;
i1;
int h=size(i);
h;
size(i1);
int s;
for(s=1;s<=h;s=s+1)
{
  NF(i1[s],i);
}
for(s=1;s<=h;s=s+1)
{
  NF(i[s],i1);
}
kill s4,h,s;

int m,n;
ring r1 = 0,(z,y,x),ds;
poly s1=2x5y+7x2y4+3x2yz3;
poly s2=1x2y2z2+3z8;
poly s3=4xy5+2x2y2z3+11x10;
ideal i=s1,s2,s3;
option(noredTail);
option();
ideal j=std(i);
hilb(j,1);
option(redSB);
option();
ideal j1=std(i);
hilb(j1,1);
option(redTail);
ideal j2=std(i);
hilb(j2,1);

n=size(j);
number x1,x2;
for(m=1;m<=n;m=m+1)
{
x1=leadcoef(j1[m]);
x2=leadcoef(j2[m]);
if(x1 != x2)
{
"?????";
j2[m]=x1*j2[m];
j1[m]=x2*j1[m];
}
j1[m]-j2[m];
NF(j[m],j1);
NF(j1[m],j);
}
j2;
option(noredSB);
option(noredTail);
kill r1;

ring r4 =0,(a,b,c,d,e),ds;
n=6;
poly s1=a+b+c+d+ea;
poly s2=de+1cd+1bc+1ae+1ab;
poly s3=cde+1bcd+1ade+1abe+1abc;
poly s4=bcde+1acde+1abde+1abce+1abcd;
poly s5=abcde;
ideal i=s1,s2,s3,s4,s5,b^n,c^n,d^n,e^n;
ideal j=std(i);
option(redTail);
ideal j1=std(i);
option(redSB);
ideal j2=std(i);

n=size(j);
number x1,x2;
for(m=1;m<=n;m=m+1)
{
x1=leadcoef(j1[m]);
x2=leadcoef(j2[m]);
x1*lead(j2[m])-x2*lead(j1[m]);
x2=leadcoef(j[m]);
x1*lead(j[m])-x2*lead(j1[m]);
NF(j[m],j2);
NF(j2[m],j);
}
j2;

kill r4;

option(noredTail);
ring r=
0,(x,y,z),ls;
poly f=x5+y11+xy9+x3y9+z6-z2x2y2;
ideal i=jacob(f);
ideal j=std(i);
option(redTail);
ideal j1=std(i);
n=size(j);
number x1,x2;
for(m=1;m<=n;m=m+1)
{
x1=leadcoef(j1[m]);
x2=leadcoef(j[m]);
x1*lead(j[m])-x2*lead(j1[m]);
}
option(noredSB);

ring r1 = 0,(z,y,x),dp;
ideal i = x2y2-z4,y2z2-x4;
i=std(i);
qring s=i;
poly s1=2x5y+7x2y3;
poly s2=1x2y2+3z7;
poly s3=4xy5+2x2yz3;
poly s4=x6+y6+z6;
module m0=
s1*gen(1)+s4*gen(2),s2*gen(2)+s3*gen(3),
s2*gen(1)+s1*gen(2),s3*gen(2)-s4*gen(3);
option(noredTail);
module m1=std(m0);
option(redSB);
option(redTail);
module m2=std(m0);
m1;
m2;
int cc;
for(cc=1;cc<31;cc=cc+1)
{
NF(m1[cc],m2);
NF(m2[cc],m1);
}

$
