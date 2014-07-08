LIB "tst.lib"; tst_init();

LIB("primdecint.lib");
ring rng = integer,(x, y, z),dp;
ideal j = 2*x, y*z+4, x*z, 2*y^2-6*z^2, 3*z^3+4*y;

def rJ = radicalZ(j);

primdecZ(rJ);

tst_status(1);$
