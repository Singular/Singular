LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id$");
LIB "ncfactor.lib";

def R = makeUsl2();
setring(R);
poly p = e^3*f+e^2*f^2-e^3+e^2*f+2*e*f^2-3*e^2*h-2*e*f*h-8*e^2
         +e*f+f^2-4*e*h-2*f*h-7*e+f-h;
list lsng = ncfactor(p);
lsng;
testNCfac(lsng, p);

tst_status();
tst_status(1); $