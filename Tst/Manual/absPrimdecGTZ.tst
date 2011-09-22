LIB "tst.lib"; tst_init();
LIB "primdec.lib";
ring  r = 0,(x,y,z),lp;
poly  p = z2+1;
poly  q = z3+2;
ideal i = p*q^2,y-z2;
def S = absPrimdecGTZ(i);
setring S;
absolute_primes;
tst_status(1);$
