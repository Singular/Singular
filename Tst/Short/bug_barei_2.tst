LIB "tst.lib";
tst_init();

option(prot);
int d=13;
int b=3;
int db=d-b;
int i;
ring r=0,(x,y,z),(c,dp);
module mm;
poly xd;
d=9;
b=3;
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

tst_status(1);$
$
