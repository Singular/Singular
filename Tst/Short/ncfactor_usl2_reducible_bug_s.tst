LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id$");
LIB "ncfactor.lib";

def R = makeUsl2();
setring(R);
poly p = -4*e*f^4*h+5*e^2*f^3+24*e*f^4;
list lsng = ncfactor(p);
lsng;
testNCfac(lsng, p);

tst_status();
tst_status(1); $