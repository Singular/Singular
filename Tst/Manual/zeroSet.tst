LIB "tst.lib"; tst_init();
LIB "zeroset.lib";
ring R = (0,a), (x,y,z), lp;
minpoly = a2 + 1;
ideal I = x2 - 1/2, a*z - 1, y - 2;
def T = zeroSet(I);
setring T;
minpoly;
newA;
id;
theZeroset;
map F1 = basering, theZeroset[1];
map F2 = basering, theZeroset[2];
F1(id);
F2(id);
tst_status(1);$
