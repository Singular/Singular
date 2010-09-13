//
// test script for bareiss command
//
LIB "tst.lib";
tst_init();

ring r = 32003,(x,y,z),dp;
r;
matrix m1[3][3]=x,2,3,2,3x,4,1,3,2x;
m1;
list mm1=bareiss(m1);
mm1;
vector vdet=mm1[1][3];
poly pdet=det(m1);
if(leadcoef(pdet)!=leadcoef(vdet))
{
  vdet+pdet*gen(3);
}
else
{
  "error";
  vdet-pdet*gen(3);
}
kill r;
"------------------------------------";
//
ring r = 0,(x,y,z),(c,dp);
matrix m2[5][6]=0,2,1,-3,12,-9,1,2,5,3,6,-1,2,4,10,6,12,-2,-3,2,1;
module x2=m2;
list mm2=bareiss(x2);
mm2;
x2=transpose(m2);
mm2=bareiss(x2);
mm2;
kill r;
"------------------------------------";
//
ring r = 0,(x,y,z),(c,lp);
matrix m3[3][4]=xy,0,x2z,1,z2y,y2z21,0,1,zx3,1,0,z3y3;
matrix m33=transpose(m3);
list mm3=bareiss(m33);
mm3;
mm3=bareiss(m3);
print(mm3[1]);
kill r;
"-------------------------------------";
//
ring r=0,(x,y,z),(c,dp);
poly s1 = 2x3y2+23x5y+104xy6+56z2x;
poly s2 = 3/2x3y2-2/3z6y4+12x2z4;
poly s3 = -2x4y5+x4z5;
poly s4 = 7/8x7y3z-1;
poly s5 = 4x2y3-2x2y2;
poly s6 = 1/2y2x3+4x2z2;
poly s7 = x+y+z;
poly s8 = x2+y2+z2;
poly s9 = xz+xy+yz;
matrix m4[3][3]=s1,0,s3,1,s5,0,s7,s8,s9;
print(m4);
list mm4=bareiss(m4);
mm4;
vector vdet=mm4[1][3];
poly pdet=det(m4);
if(leadcoef(pdet)!=leadcoef(vdet))
{
  vdet+pdet*gen(3);
}
else
{
  "error";
  vdet-pdet*gen(3);
}
"---------------------------------------";
//
matrix m5[3][2]=s8,s7,s9,s6,s5,0;
list mm5=bareiss(m5);
print(mm5[1]);
kill r;
"-------------------------------------";
int n=5;
int nn=n^2;
ring r1=0,(x(1..nn)),dp;
matrix m6[n][n]=maxideal(1);
list mm6=bareiss(m6);
vector p1=mm6[1][n];
poly p2=det(m6);
if(leadcoef(p1)!=leadcoef(p2))
{
  "error";
  p1+p2*gen(n);
}
else
{
  p1-p2*gen(n);
}
"--------------------------------------";
ring r2=0,(x(1..12)),ds;
matrix m7[3][4]= maxideal(1);
print(m7);
list mm7=bareiss(m7);
print(mm7[1]);
"--------------------------------------";
matrix m8[4][3]=maxideal(1);
print(m8);
list mm8=bareiss(m8);
print(mm8[1]);
"--------------------------------------";
matrix m9[4][4]=maxideal(2);
print(m9);
list mm9=bareiss(m9);
print(mm9[1]);
mm9[1][4];
"------------------------------------";
matrix m11[5][5]=maxideal(1),maxideal(1);
print(m11);
list mm11=bareiss(m11);
vector p1=mm11[1][5];
poly p2=det(m11);
if(leadcoef(p1)!=leadcoef(p2))
{
  "error";
  p1+p2*gen(5);
}
else
{
  p1-p2*gen(5);
}
kill r1,r2;
"-------------------------------------";
ring r=32003,(x,y,z),(c,dp);
module m;
int d=64;
int b=8;
int db=d-b;
int i;
for(i=d;i>0;i--)
{
  m[i] = x*gen(i);
}
for(i=db;i>0;i--)
{
  m[i]=m[i]+y*gen(i+b);
}
for(i=d;i>db;i--)
{
  m[i]=m[i]+y*gen(i-db);
}
for(i=d;i>b;i--)
{
  m[i]=m[i]+z*gen(i-b);
}
for(i=b;i>0;i--)
{
  m[i]=m[i]+z*gen(i+db);
}
list rs=bareiss(m);
rs[1][d];
tst_status(1);$
