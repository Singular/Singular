LIB "tst.lib"; tst_init();
LIB "elim.lib";
ring r  = 0,(x,y),dp;
poly f  = x2+y3;
ideal C = x,y;           //center of blowup
def B1 = blowup0(f,C);
setring B1;
aS;                      //ideal of blown up ambient space
tT;                      //ideal of total transform of f
sT;                      //ideal of strict transform of f
eD;                      //ideal of exceptional divisor
bM;                      //ideal of blowup map r --> B1
ring R  = 0,(x,y,z),ds;
poly f  = y2+x3+z5;
ideal C = y2,x,z;
ideal W = z-x;
def B2 = blowup0(f,C,W);
setring B2;
B2;                       //weighted ordering
bR;                       //ideal of blown up R
aS;                       //ideal of blown up R/W
sT;                       //strict transform of f
eD;                       //ideal of exceptional divisor
//Note that the different affine charts are {y(i)=1}
tst_status(1);$
