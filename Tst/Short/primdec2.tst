LIB "tst.lib";
tst_init();

LIB"primdec.lib";
ring r=32003,(a,b,c,d,e,f),(C,dp);
ideal i=
a2cdf2,
b2cdf2,
a2bdf2,
b3df2,
a3df2,
ab2df2,
a2cde,
b2cde,
a2bde,
b3de,
a3de,
ab2de,
a2cd2,
b2cd2,
a2bd2,
b3d2,
a3d2,
ab2d2,
a2c2f2,
b2c2f2,
a2bcf2,
b3cf2,
a3cf2,
ab2cf2,
a2b2f2,
b4f2,
a3bf2,
ab3f2,
a4f2;
 equidimMax(i);
 equidimMaxEHV(i);
 equidim(i);
 equidim(i,1);

ring r=32003,(a,b,c,d,e,f,g,h,j),(C,dp);
ideal i=
a+2b+c-d+g,
f2gh-a,
efg-c,
fg2j-b,
a+b+c+f+g-1,
3ad+3bd+2cd+df+dg-a-2b-c-g;

 equidimMax(i);
 equidimMaxEHV(i);
 equidim(i);
 equidim(i,1);


ring r=32003,(b,s,t,u,v,w,x,y,z),(C,dp);
ideal i=
bv+su,
bw+tu,
sw+tv,
by+sx,
bz+tx,
sz+ty,
uy+vx,
uz+wx,
vz+wy;

 equidimMax(i);
 equidimMaxEHV(i);
 equidim(i);
 equidim(i,1);

ring r=32003,(x,y,z),(C,dp);
ideal i=
xy2z2-1xy2z+xyz2-1xyz,
xy3z+xy2z,
xy4-1xy2,
x2yz2-1x2yz,
x2y3-1x2y2,
x4z3-1x4z2+2x3z3-2x3z2+x2z3-1x2z2,
x2y2z,
x4yz+x3yz,
2x4y2+6x3y2+6x2y2+xy3+xy2,
x5z+x4z2+x4z+2x3z2-1x3z+x2z2-1x2z,
x6y+3x5y+3x4y+x3y;

 equidimMax(i);
 equidimMaxEHV(i);
 equidim(i);
 equidim(i,1);

ring r=32003,(b,s,t,u,v,w,x,y,z),(C,dp);
ideal i=
su-bv,
tv-sw,
vx-uy,
wy-vz;

 equidimMax(i);
 equidimMaxEHV(i);
 equidim(i);
 equidim(i,1);


ring r =32003,(a,b,c,d),(C,dp);
ideal i=
a+b+c+d,
1cd+1bc+1ab+ad,
1bcd+1abc+abd+acd,
1abcd-1;

 equidimMax(i);
 equidimMaxEHV(i);
 equidim(i);
 equidim(i,1);

tst_status(1);$
