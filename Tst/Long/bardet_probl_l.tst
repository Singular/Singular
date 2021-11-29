LIB "tst.lib";
tst_init();
/*
*  Probleme:
*   1. "minor" verwendet "det" mit Matrizen,
*      das sollte wegen sparse ueber Moduln laufen.
*   2. Rechenzeit von "bareiss" >> "det", Brueche und Normalisierung?
*      genaue Zeitmessung erforderlich.
*   3. Sparse Matrizen ohne Symbole sollten nicht nach factory.
*/
// 1.
ring r=0,(x,y,z,w),dp;
module mm;
poly xd;
int d=13;
int b=3;
int db=d-b;
int i;
for(i=d;i>0;i--)
{
  mm[i]=3*x*gen(i)+w*gen(i);
}
for(i=db;i;i--)
{
  mm[i]=mm[i]+7*y*gen(i+b)+w*gen(i+b);
}
for(i=d;i>db;i--)
{
  mm[i]=mm[i]+7*y*gen(i-db)+w*gen(i-db);
}
for(i=d;i>b;i--)
{
  mm[i]=mm[i]+11*z*gen(i-b)+w*gen(i-b);
}
for(i=b;i;i--)
{
  mm[i]=mm[i]+11*z*gen(i+db)+w*gen(i+db);
}
matrix nn=mm;
ideal dd=minor(nn,d-1);
size(dd);
ideal ee=minor(nn,5);
size(ee);
kill r;
// 2.
ring r=0,(x,y,z),(c,dp);
module mm;
poly xd;
d=39;
b=13;
db=d-b;
for(i=d;i>0;i--)
{
  mm[i]=1/3*x*gen(i);
}
for(i=db;i;i--)
{
  mm[i]=mm[i]+1/7*y*gen(i+b);
}
for(i=d;i>db;i--)
{
  mm[i]=mm[i]+1/7*y*gen(i-db);
}
for(i=d;i>b;i--)
{
  mm[i]=mm[i]+1/11*z*gen(i-b);
}
for(i=b;i;i--)
{
  mm[i]=mm[i]+1/11*z*gen(i+db);
}
list ss=bareiss(mm);
vector vv=ss[1][size(ss[1])];
xd=det(mm);
vector ww=xd*gen(d);
ww-vv;
xd;
kill r;
// 3.
ring r=0,(x,y,z),(c,dp);
module mm;
poly xd;
d=390;
b=13;
db=d-b;
for(i=d;i>0;i--)
{
  mm[i]=77*gen(i);
}
for(i=db;i;i--)
{
  mm[i]=mm[i]+33*gen(i+b);
}
for(i=d;i>db;i--)
{
  mm[i]=mm[i]+33*gen(i-db);
}
for(i=d;i>b;i--)
{
  mm[i]=mm[i]+21*gen(i-b);
}
for(i=b;i;i--)
{
  mm[i]=mm[i]+21*gen(i+db);
}
xd=det(mm);
matrix nn=mm;
poly dd=det(nn);
xd-dd;
dd;
kill r;
tst_status(1); $

