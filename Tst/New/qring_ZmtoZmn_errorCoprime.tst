LIB "tst.lib";
tst_init();

ring rng = (integer,77),(x,y,z),dp;

ideal iq = 20,2*y, 4*x*z+3*y, y^2-2*x*z-y;
iq = std(iq);
iq;

qring rngQ = iq;

ideal J = 3, y-4, x*z;

ideal stdJ =  groebner(J);
stdJ;
def reduceRes = reduce( J, stdJ);

std(reduceRes);
rngQ;


tst_status(1);
$
