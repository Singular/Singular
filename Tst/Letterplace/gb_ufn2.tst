LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(a,b,c,d,e),Dp;
int upToDeg = 17;
def R = makeLetterplaceRing(upToDeg);
setring(R);
ideal Id = a*a-a,
b*b-b,
c*c-c,
d*d-d,
e*e-e,
a*b*a-a*b,
b*a*b-a*b,
a*c*a-a*c,
c*a*c-a*c,
a*d*a-a*d,
d*a*d-a*d,
a*e*a-a*e,
e*a*e-a*e,
b*c*b-b*c,
c*b*c-b*c,
b*d*b-b*d,
d*b*d-b*d,
b*e*b-b*e,
e*b*e-b*e,
c*d*c-c*d,
d*c*d-c*d,
c*e*c-c*e,
e*c*e-e*c,
e*d*e-d*e,
d*e*d-d*e;
option(redTail);
option(redSB);
std(Id);
tst_status(1);$
