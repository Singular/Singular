LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(f1,f2,h1,h2,e1,e2),Dp;
int upToDeg = 8;
def R = makeLetterplaceRing(upToDeg);
setring(R);
ideal Id = f1*f2*f2-2*f2*f1*f2+f2*f2*f1,
f1*f1*f1*f1*f2-4*f1*f1*f1*f2*f1+6*f1*f1*f2*f1*f1-4*f1*f2*f1*f1*f1+f2*f1*f1*f1*f1,
e1*e2*e2-2*e2*e1*e2+e2*e2*e1,
e1*e1*e1*e1*e2-4*e1*e1*e1*e2*e1+6*e1*e1*e2*e1*e1-4*e1*e2*e1*e1*e1+e2*e1*e1*e1*e1,
f2*e1-e1*f2,
f1*e2-e2*f1,
-f1*e1+e1*f1-h1,
-f2*e2+e2*f2-h2,
h1*h2-h2*h1,
h1*e1-e1*h1-2*e1,
f1*h1-h1*f1-2*f1,
h1*e2-e2*h1+e2,
f2*h1-h1*f2+f2,
h2*e1-e1*h2+3*e1,
f1*h2-h2*f1+3*f1,
h2*e2-e2*h2-2*e2,
f2*h2-h2*f2-2*f2;
option(redTail);
option(redSB);
std(Id);
tst_status(1);$
