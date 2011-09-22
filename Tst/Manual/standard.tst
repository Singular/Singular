LIB "tst.lib"; tst_init();
LIB "teachstd.lib";
ring r=0,(x,y,z),dp;
ideal G = x2y+x2,y3+xyz,xyz2+z4;
standard(G);"";
ring s=0,(x,y,z),(c,ds);
ideal G = 2x2+x2y,y3+xyz,3x3y+z4;
standard(G);"";
standard(G,1);"";           //use symmetric s-poly without division
module M = [2x2,x3y+z4],[3y3+xyz,y3],[5z4,z2];
standard(M);
tst_status(1);$
