//Tests fuer std  mit redtail
"              Testbeispiele mit redtail, char=0";
//timer=1;
//test(0);
int m,n;

ring r1 = 0,(z,y,x),ds;
poly s1=2x5y+7x2y4+3x2yz3;
poly s2=1x2y2z2+3z8;
poly s3=4xy5+2x2y2z3+11x10;
ideal i=s1,s2,s3;
ideal j=std(i);
hilb(j,1);
option(redSB);
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
NF(x2*j1[m]-x1*j2[m],j);
NF(j[m],j1);
NF(j1[m],j);
}
j2;
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

option(redSB);
ring r=
0,(x,y,z),ls;
poly f=x5+y11+xy9+x3y9+z6-z2x2y2;
ideal i=jacob(f);
ideal j=std( i);
j;
hilb(j);
ring r1 = 0,(z,y,x),dp;
ideal i = x4y4,y4z4;
i=std(i);
qring s=i;
poly s1=2x5y+7x2y3;
poly s2=1x2y2+3z7;
poly s3=4xy5+2x2yz3;
poly s4=x6+y6+z6;
module M=
s1*gen(1)+s4*gen(2),s2*gen(2)+s3*gen(3),
s2*gen(1)+s1*gen(2),s3*gen(2)-s4*gen(3),
s3*gen(1)+s2*gen(2),s1*gen(1)-s4*gen(3);
option(redSB);
module m1=std(M);
option(redTail);
option(noredSB);
module m2=std(M);
int cc;
for(cc=1;cc<47;cc=cc+1)
{
leadcoef(m2[cc])*lead(m1[cc])-leadcoef(m1[cc])*lead(m2[cc]);
NF(m1[cc],m2);
NF(m2[cc],m1);
}

$

