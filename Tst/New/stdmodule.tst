LIB "tst.lib";
tst_init();

LIB "stdmodule.lib";
ring r=0,(x,y,z),Dp;
ideal A=x2-yz,xy+x,xz;
ideal A=x2-yz, xy+x, xz;
module G=[x2yz+x2z-2,xz+xyz2-x3z],[x3y-xy2z+x3-xyz,-1],[xy+x,x2z2-x2+yz];
MPGS(I,A);
MSPOLY(I,A);
MSGB(I,A);

ring r2=0,(x,y),Dp;
ideal A=x2, y2;
module G=[x4+2x2y2-1,y4],[-x2y2,x2y2],[2x2y4-y2,x4y2+y6],[-x2y2,x6y2+2x4y4+x2y6];
MPGS(G,A);
MSPOLY(G,A);
MSGB(G,A);

example MSGB;
example MSPOLY;
example MPGS;
example SNF;

tst_status(1);$
