LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(H2,H1,y3,y2,y1,x3,x2,x1),Dp;
int upToDeg = 5;
def R = makeLetterplaceRing(upToDeg);
setring(R);
ideal Id = x2*x1-x1*x2-x3,
x3*x1-x1*x3,
y1*x1-x1*y1+H1,
y2*x1-x1*y2,
y3*x1-x1*y3+y2,
H1*x1-x1*H1-2*x1,
H2*x1-x1*H2+x1,
x3*x2-x2*x3,
y1*x2-x2*y1,
y2*x2-x2*y2+H2,
y3*x2-x2*y3-y1,
H1*x2-x2*H1+x2,
H2*x2-x2*H2-2*x2,
y1*x3-x3*y1+x2,
y2*x3-x3*y2-x1,
y3*x3-x3*y3+H1+H2,
H1*x3-x3*H1-x3,
H2*x3-x3*H2-x3,
y2*y1-y1*y2+y3,
y3*y1-y1*y3,
H1*y1-y1*H1+2*y1,
H2*y1-y1*H2-y1,
y3*y2-y2*y3,
H1*y2-y2*H1-y2,
H2*y2-y2*H2+2*y2,
H1*y3-y3*H1+y3,
H2*y3-y3*H2+y3,
H2*H1-H1*H2,
3*x1*y1+3*x2*y2+3*x3*y3+H1*H1+H1*H2+H2*H2-3*H1-3*H2,
27*x3*y1*y2+27*x1*x2*y3-9*x1*y1*H1+18*x2*y2*H1-9*x3*y3*H1-2*H1*H1*H1-18*x1*y1*H2+9*x2*y2*H2+9*x3*y3*H2-3*H1*H1*H2+3*H1*H2*H2+2*H2*H2*H2-54*x2*y2-27*x3*y3-9*H1*H2-18*H2*H2+18*H1+36*H2;
option(redTail);
option(redSB);
std(Id);
tst_status(1);$
