bigintvec hi;
ring r0=0,(x,y,z),dp;
module mm=
(x2z2-xy2z)*gen(1)-(x2zy2+xyz3)*gen(3)+(xy2+xyz)*gen(2),
(xy+yz)*gen(1)-(xz2-xyz)*gen(3)+(y+z)*gen(2),
(x3y2z+xy3z2)*gen(1)-(x3y2+y3z2)*gen(2),
(x3+y3+z3)*gen(1)+(x3y-y2z2)*gen(3)-(xy+yz+xz)*gen(2);
module nn=std(mm);
attrib(nn,"isHomog");
intvec we=-9,-8,-10;
hi=hilb(nn,1);
hi;
ring r1=0,(x,y,z),(C,lp);
module mm=fetch(r0,mm);
module nn=std(mm);
module nm=std(mm,hi);
int i;
for(i=1;i<=size(nn);i++)
{
  nn[i]-nm[i];
}
setring r0;
attrib(nn,"isHomog",we);
hi=hilb(nn,1);
hi;
setring r1;
mm=fetch(r0,mm);
attrib(mm,"isHomog",we);
nm=std(mm,hi);
for(i=1;i<=size(nn);i++)
{
  nn[i]-nm[i];
}
LIB "tst.lib";tst_status(1);$

