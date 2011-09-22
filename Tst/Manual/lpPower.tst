LIB "tst.lib"; tst_init();
LIB "freegb.lib";
// define a ring in letterplace form as follows:
ring r = 0,(x(1),y(1),x(2),y(2),x(3),y(3),x(4),y(4)),dp;
def R = setLetterplaceAttributes(r,4,2); // supply R with letterplace structure
setring R;
poly a = x(1)*y(2) + y(1); poly b = y(1) - 1;
lpPower(a,2);
lpPower(b,4);
tst_status(1);$
