LIB "tst.lib"; tst_init();
LIB "primdec.lib";
ring  r = 32003,(x,y,z),dp;
poly  p = z2+1;
poly  q = z4+2;
ideal i = p^2*q^3,(y-z3)^3,(x-yz+z4)^4;
list pr = primdecGTZ(i);
testPrimary(pr,i);
tst_status(1);$
