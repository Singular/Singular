LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def R = makeUsl2();
// this algebra is U(sl_2)
setring R;
poly C  = e*f*h;
poly C1 = subst(C,e,h^3);
C1;
poly C2 = subst(C,f,e+f);
C2;
tst_status(1);$
