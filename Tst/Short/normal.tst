LIB "tst.lib";
tst_init();

LIB"normal.lib";

ring r=31991,(a,b,c,d,e),dp;
ideal i=
5abcde-a5-b5-c5-d5-e5,
ab3c+bc3d+a3be+cd3e+ade3,
a2bc2+b2cd2+a2d2e+ab2e2+c2de2,
abc5-b4c2d-2a2b2cde+ac3d2e-a4de2+bcd2e3+abe5,
ab2c4-b5cd-a2b3de+2abc2d2e+ad4e2-a2bce3-cde5,
a3b2cd-bc2d4+ab2c3e-b5de-d6e+3abcd2e2-a2be4-de6,
a4b2c-abc2d3-ab5e-b3c2de-ad5e+2a2bcde2+cd2e4,
b6c+bc6+a2b4e-3ab2c2de+c4d2e-a3cde2-abd3e2+bce5;

list pr=normal(i);
pr;
def r1=pr[1];
setring r1;
norid; normap;
kill r,r1;

ring r=32003,(x,y,z),wp(2,3,6);
ideal i=zy2-zx3-x6;
list pr=normal(i);
pr;
def r1=pr[1];
setring r1;
norid; normap;
kill r,r1;

ring r=32003,(x,y,z),dp;
ideal i=(x-y)*(x-z)*(y-z);

list pr=normal(i);
pr;
def r1=pr[1];
def r2=pr[2];
setring r1;
norid; normap;
kill r,r1;

ring r=32003,(b,s,t,u,v,w,x,y,z),dp;

ideal k=
wy-vz,
vx-uy,
tv-sw,
su-bv,
tuy-bvz;
ideal j=x2y2+x2z2+y2z2;
ideal i=mstd(intersect(j,k))[2];
list pr=normal(i);
pr;
def r1=pr[1];
def r4=pr[4];
setring r1;
norid; normap;
setring r4;
norid; normap;

kill r,r1;


example deltaLoc;
example genus; 

tst_status(1);$


// This is the example Theo 3 which is still broken due to the broken
// minor command
ring r=32003,(x,y,z),wp(3,5,15);
ideal i=z*(y3-x5)+x10;
list pr=normal(i);
pr;
def r1=pr[1];
setring r1;
norid; normap;
kill r, r1;
