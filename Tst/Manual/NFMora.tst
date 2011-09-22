LIB "tst.lib"; tst_init();
LIB "teachstd.lib";
ring r=0,(x,y,z),dp;
poly f = x2y2z2+x5+yx3+z6-3y3;
ideal G = x2y+x2,y3+xyz,xyz2+z6;
NFMora(f,G);"";
ring s=0,(x,y,z),ds;
poly f = x3y+x5+x2y2z2+z6;
ideal G = x2+x2y,y3+xyz,x3y2+z4;
NFMora(f,G);"";
vector v = [f,x2+x2y];
module M = [x2+x2y,f],[y3+xyz,y3],[x3y2+z4,z2];
NFMora(v,M);
tst_status(1);$
