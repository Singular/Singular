LIB "tst.lib";
tst_init();

// reduce/std over qring with ring-cf:
ring rng = integer,(x,y,z),dp;
ideal iq = 2*y+1, 4*x*z+3*y, y^2-2*x*z-y;
iq = std(iq);

qring rngQ = iq;

ideal J = 3, y-4, x*z;

ideal stdJ =  groebner(J);
stdJ;
def reduceRes = reduce( J, stdJ);
reduceRes;

tst_status(1);$
