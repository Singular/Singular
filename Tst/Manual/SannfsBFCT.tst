LIB "tst.lib"; tst_init();
LIB "dmod.lib";
ring r = 0,(x,y,z,w),Dp;
poly F = x^3+y^3+z^3*w;
// compute Ann(F^s)+<F> using slimgb only
def A = SannfsBFCT(F);
setring A; A;
LD;
// the Bernstein-Sato poly of F:
vec2poly(pIntersect(s,LD));
// a fancier example:
def R = reiffen(4,5); setring R;
RC; // the Reiffen curve in 4,5
// compute Ann(RC^s)+<RC,diff(RC,x),diff(RC,y)>
// using std for GB computations of ideals <I+J>
// where I is already a GB of <I>
// and slimgb for other ideals
def B = SannfsBFCT(RC,1,0,1);
setring B;
// the Bernstein-Sato poly of RC:
(s-1)*vec2poly(pIntersect(s,LD));
tst_status(1);$
