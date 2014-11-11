LIB "tst.lib";
tst_init();


LIB("primdec.lib");
ring rng = 0,(x, y, z),dp;

ideal I = 4*y^3-8*z, 4*x*y^2+14*x*y*z+13*z^2-6, 15*x^2*y+15*x^2*z-24*y*z;


def eIMEHV = equidimMaxEHV(I);
def eIM = equidimMax(I);

ASSUME(0, idealsEqual(eIMEHV,eIM));


tst_status(1); $
