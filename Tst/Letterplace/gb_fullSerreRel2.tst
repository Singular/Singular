LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(f1,f2,f3,h1,h2,h3,e1,e2,e3),Dp;
int upToDeg = 5;
def R = makeLetterplaceRing(upToDeg);
setring(R);
ideal Id = f1*f2*f2-2*f2*f1*f2+f2*f2*f1,
f1*f3-f3*f1,
f1*f1*f2-2*f1*f2*f1+f2*f1*f1,
f2*f3*f3*f3*f3-4*f3*f2*f3*f3*f3+6*f3*f3*f2*f3*f3-4*f3*f3*f3*f2*f3+f3*f3*f3*f3*f2,
f2*f2*f3-2*f2*f3*f2+f3*f2*f2,
e1*e2*e2-2*e2*e1*e2+e2*e2*e1,
e1*e3-e3*e1,
e1*e1*e2-2*e1*e2*e1+e2*e1*e1,
e2*e3*e3*e3*e3-4*e3*e2*e3*e3*e3+6*e3*e3*e2*e3*e3-4*e3*e3*e3*e2*e3+e3*e3*e3*e3*e2,
e2*e2*e3-2*e2*e3*e2+e3*e2*e2,
f2*e1-e1*f2,
f1*e2-e2*f1,
f3*e1-e1*f3,
f1*e3-e3*f1,
f3*e2-e2*f3,
f2*e3-e3*f2,
-f1*e1+e1*f1-h1,
-f2*e2+e2*f2-h2,
-f3*e3+e3*f3-h3,
h1*h2-h2*h1,
h1*h3-h3*h1,
h2*h3-h3*h2,
h1*e1-e1*h1-2*e1,
f1*h1-h1*f1-2*f1,
h1*e2-e2*h1+e2,
f2*h1-h1*f2+f2,
h1*e3-e3*h1,
f3*h1-h1*f3,
h2*e1-e1*h2+e1,
f1*h2-h2*f1+f1,
h2*e2-e2*h2-2*e2,
f2*h2-h2*f2-2*f2,
h2*e3-e3*h2+3*e3,
f3*h2-h2*f3+3*f3,
h3*e1-e1*h3,
f1*h3-h3*f1,
h3*e2-e2*h3+e2,
f2*h3-h3*f2+f2,
h3*e3-e3*h3-2*e3,
f3*h3-h3*f3-2*f3;
option(redTail);
option(redSB);
std(Id);
tst_status(1);$
