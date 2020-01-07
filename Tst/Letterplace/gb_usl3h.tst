LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x1,x2,x3,y1,y2,y3,H1,H2,h),Dp;
int upToDeg = 4;
def R = makeLetterplaceRing(upToDeg);
setring(R);
ideal Id = x2*x1-x1*x2-x3*h,
x3*x1-x1*x3,
y1*x1-x1*y1+H1*h,
y2*x1-x1*y2,
y3*x1-x1*y3+y2*h,
H1*x1-x1*H1-2*x1*h,
H2*x1-x1*H2+x1*h,
x3*x2-x2*x3,
y1*x2-x2*y1,
y2*x2-x2*y2+H2*h,
y3*x2-x2*y3-y1*h,
H1*x2-x2*H1+x2*h,
H2*x2-x2*H2-2*x2*h,
y1*x3-x3*y1+x2*h,
y2*x3-x3*y2-x1*h,
y3*x3-x3*y3+H1*h+H2*h,
H1*x3-x3*H1-x3*h,
H2*x3-x3*H2-x3*h,
y2*y1-y1*y2+y3*h,
y3*y1-y1*y3,
H1*y1-y1*H1+2*y1*h,
H2*y1-y1*H2-y1*h,
y3*y2-y2*y3,
H1*y2-y2*H1-y2*h,
H2*y2-y2*H2+2*y2*h,
H1*y3-y3*H1+y3*h,
H2*y3-y3*H2+y3*h,
H2*H1-H1*H2,
x1*h-h*x1,
x2*h-h*x2,
x3*h-h*x3,
y1*h-h*y1,
y2*h-h*y2,
y3*h-h*y3,
H1*h-h*H1,
H2*h-h*H2;
option(redTail);
option(redSB);
std(Id);
tst_status(1);$
