LIB "tst.lib"; tst_init();
LIB "primitiv.lib";
ring r=0,(x,y),dp;
def r1=splitring(x2-2);
setring r1; basering;    // change to Q(sqrt(2))
// change to Q(sqrt(2),sqrt(sqrt(2)))=Q(a) and return the transformed
// old parameter:
def r2=splitring(x2-a,a);
setring r2; basering; erg;
// the result is (a)^2 = (sqrt(sqrt(2)))^2
kill r1; kill r2;
tst_status(1);$
