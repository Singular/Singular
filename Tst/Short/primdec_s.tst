LIB "tst.lib";
tst_init();

LIB "primdec.lib";

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

list pr1=minAssGTZ(i);
ideal i1=radical(i);
ideal i2=equiRadical(i);
list pr2= primdecGTZ(i);
list pr3=minAssChar(i,1);
list pr4=primdecSY(i,1);
matrix m=char_series(i);
pr1;
i1;
i2;
pr2;
pr3;
pr4;
m;
testPrimary(pr2,i);
kill r;

ring r=0,(a,b,c,d,e,f),(C,dp);
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

list pr1=minAssGTZ(i);
ideal i1=radical(i);
ideal i2=equiRadical(i);
list pr2= primdecGTZ(i);
list pr3=minAssChar(i,1);
list pr4=primdecSY(i,1);
matrix m=char_series(i);
pr1;
i1;
i2;
pr2;
pr3;
pr4;
m;
testPrimary(pr2,i);
kill r;



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

list pr1=minAssGTZ(i);
ideal i1=radical(i);
ideal i2=equiRadical(i);
list pr2= primdecGTZ(i);
list pr3=minAssChar(i,1);
list pr4=primdecSY(i,1);
matrix m=char_series(i);
pr1;
i1;
i2;
pr2;
pr3;
pr4;
m;
testPrimary(pr2,i);
kill r;


ring r=0,(x,y,z),(C,dp);
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

list pr1=minAssGTZ(i);
ideal i1=radical(i);
ideal i2=equiRadical(i);
list pr2= primdecGTZ(i);
list pr3=minAssChar(i,1);
list pr4=primdecSY(i,1);
matrix m=char_series(i);
pr1;
i1;
i2;
pr2;
pr3;
pr4;
m;
testPrimary(pr2,i);
kill r;

ring r = 32003,(b,c,d,e,f,g,h,j,k,l),(C,dp); 
ideal i= 
(l-k)^9,
(l-k)^8*(l-b),
(l-k)^7*(l-c),
(l-k)^6*(l-d),
(l-k)^5*(l-e),
(l-k)^4*(l-f),
(l-k)^3*(l-g),
(l-k)^2*(l-h),
(l-k)*(l-j);

list pr1=minAssGTZ(i);
ideal i1=radical(i);
ideal i2=equiRadical(i);
list pr2= primdecGTZ(i);
list pr3=minAssChar(i,1);
list pr4=primdecSY(i,1);
matrix m=char_series(i);
pr1;
i1;
i2;
pr2;
pr3;
pr4;
m;
testPrimary(pr2,i);
kill r;

// new example from Gerhard/Agnes
ring R=3,(x,y),lp;
ideal i=y4-x3-x,x9-x,y9-y;
list pr1=minAssGTZ(i);
ideal i1=radical(i);
ideal i2=equiRadical(i);
list pr2= primdecGTZ(i);
list pr3=minAssChar(i,1);
list pr4=primdecSY(i,1);
matrix m=char_series(i);
pr1;
i1;
i2;
pr2;
pr3;
pr4;
m;
testPrimary(pr2,i);

tst_status(1);$
