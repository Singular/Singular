LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id$");
option(prot);
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
std(i);
facstd(i);

 ring r=32003,(x,y,z,u,v),(C,dp);
 poly p=x4+y4+z4+u4+v4+(x+y+z+u+v)^4;
 ideal i=jacob(p);
std(i);
facstd(i);



ring r=32003,(a,b,c,d,e,f,g,h,j),(C,dp);
ideal i=
a+2b+c-d+g,
f2gh-a,
efg-c,
fg2j-b,
a+b+c+f+g-1,
3ad+3bd+2cd+df+dg-a-2b-c-g;
std(i);
facstd(i);



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
std(i);
facstd(i);

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
std(i);
facstd(i);



ring r=32003,(a,b,c,d,e,f),(C,dp);
ideal i=
adef+3/2be2f-1/2cef2,
ad2f+5/4bdef+1/4cdf2,
ad2e+3/4bde2+7/4cdef,
acde+3/2bce2-1/2c2ef,
acd2+5/4bcde+1/4c2df,
abdf+3/2b2ef-1/2bcf2,
abde+3/4b2e2-1acdf+1/2bcef-1/4c2f2,
abd2+3/4b2de+7/4bcdf,
abcd+3/2b2ce-1/2bc2f,
a2df+5/4abef+1/4acf2,
a2de+3/4abe2+7/4acef,
a2d2-9/16b2e2+2acdf-9/8bcef+7/16c2f2,
a2cd+5/4abce+1/4ac2f,
a2bd+3/4ab2e+7/4abcf,
bc3d+1/4cd3e,
b2c2e-1/3bc3f+2/3cd2ef,
b2c2d-1/2ad4-3/8bd3e-1/8cd3f,
b3ce-1/3b2c2f+2/3bd2ef,
b3cd+3/4bd3f,
ac3e-1/3cde3,
ac3d-3/4bc3e-1/2cd2e2+1/4c4f,
abc2f-1cdef2,
abc2e-1cde2f,
ab2cf-1bdef2,
ab3f-3bdf3,
ab3d+3/4b4e-1/4b3cf-3/2bd2f2,
a2c2e-2/3ade3-1/2be4+1/6ce3f,
a2bcf+3/2be2f2-1/2cef3,
a2bce+3/2be3f-1/2ce2f2,
a2b2f-2adf3+3/2bef3-1/2cf4,
a3ce+4/3ae3f,
a3bf+4aef3,
a4d+3/4a3be+1/4a3cf-2ae2f2;
std(i);
facstd(i);


ring r=32003,(a,b,c,d,e,h),(C,dp);
ideal i=
-1ab-1b2-2de-2ch,
-1ac-2bc-1e2-2dh,
-1c2-1ad-2bd-2eh,
-2cd-1ae-2be-1h2,
-1d2-2ce-1ah-2bh;
std(i);
facstd(i);


ring r= 23003,(t,u,w,x,y,z),lp;
ideal i=
2x2+2y2+2z2+2t2+u2-u,
xy+2yz+2zt+2tu-t,
2xz+2yt+t2+2zu-z,
2xt+2zt+2yu-y,
2x+2y+2z+2t+u-1;
groebner(i);
facstd(i);


ring r =32003,(a,b,c,d,e,h),(C,dp);
ideal i=
a+b+c+d+e,
de+1cd+1bc+1ae+1ab,
cde+1bcd+1ade+1abe+1abc,
bcde+1acde+1abde+1abce+1abcd,
abcde-h5;
std(i);
facstd(i);


ring r=0,(a,b,c,d,e,f,g,h,k,o),lp; 
ideal i= 
o+1,
k4+k,
hk,
h4+h,
gk,
gh,
g3+h3+k3+1,
fk,
f4+f,
eh,
ef,
f3h3+e3k3+e3+f3+h3+k3+1,
e3g+f3g+g,
e4+e,
dh3+dk3+d,
dg,
df,
de,
d3+e3+f3+1,
e2g2+d2h2+c,
f2g2+d2k2+b,
f2h2+e2k2+a;
groebner(i);
facstd(i);


ring r=32003,(a,b,c,d,e,f,g,h,j,k,l),(C,dp);
ideal i=
-2hjk+4ef+bj+ak,
-2hjl+4eg+cj+al,
-4fhj-4ehk-djk+2be+2af,
-4ghj-4ehl-djl+2ce+2ag,
-2dfj-2dek+ab,
-2dgj-2del+ac;
std(i);
facstd(i);

ring r=0,(t,u,v,w,x,y,z),(C,dp);
ideal i=
yw-1/2zw+tw,
-2/7uw2+10/7vw2-20/7w3+tu-5tv+10tw,
2/7yw2-2/7zw2+6/7tw2-yt+zt-3t2,
-2v3+4uvw+5v2w-6uw2-7vw2+15w3+42yv,
-14zv-63yw+21zw-42tw+147x,
-9/7uw3+45/7vw3-135/7w4+2zv2-2tv2-4zuw+10tuw-2zvw-28tvw+4zw2+86tw2-42yz
+14z2+42yt-14zt-21xu+105xv-315xw,
6/7yw3-9/7zw3+36/7tw3-2xv2-4ytw+6ztw-24t2w+4xuw+2xvw-4xw2+56xy-35xz+84xt,
2uvw-6v2w-uw2+13vw2-5w3+14yw-28tw,
u2w-3uvw+5uw2+14yw-28tw,
-2zuw-2tuw+4yvw+6zvw-2tvw-16yw2-10zw2+22tw2+42xw,
28/3yuw+8/3zuw-20/3tuw-88/3yvw-8zvw+68/3tvw+52yw2+40/3zw2-44tw2-84xw,
-4yzw+10ytw+8ztw-20t2w+12xuw-30xvw+15xw2,
-y2w+1/2yzw+ytw-ztw+2t2w-3xuw+6xvw-3xw2,
8xyw-4xzw+8xtw;
std(i);
facstd(i);



ring r=0,(a,b,c,d,u,v,w,x),lp;
ideal i=
a+b+c+d,
u+v+w+x,
3*(ab+ac+ad+bc+bd+cd)+2,
a*(v+w+x)+b*(u+w+x)+c*(u+v+x)+d*(u+v+w),
u*(bc+bd+cd)+v*(ac+ad+cd)+w*(ab+ad+bd)+x*(ab+ac+bc),
abc+abd+acd+bcd,
abcx+abwd+avcd+ubcd;
groebner(i);
facstd(i);



tst_status(1);$
