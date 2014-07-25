LIB "tst.lib";
tst_init();

LIB("primdec.lib");
ring rng = 0,(x,r,b,u),lp;
ideal I = b^3-6*b^2+5*b-1,
r^2-r*u+u^2*b+b^2-6*b+1,
x^2+2*x*r*b^2-11*x*r*b+5*x*r+r*u-u^2*b;

def L1 = primdecGTZ(I);
ASSUME(0, testPrimary(L1,I) );

list LGTZI = minAssGTZ(I);
list LCharI = minAssChar(I);
LGTZI;
LCharI;

ASSUME(0, size(LGTZI)==size(LCharI) );

tst_status(1);$

