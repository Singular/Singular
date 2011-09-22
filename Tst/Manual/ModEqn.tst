LIB "tst.lib"; tst_init();
LIB "qhmoduli.lib";
ring B   = 0,(x,y), ls;
poly f = -x4 + xy5;
def R = ModEqn(f);
setring R;
modid;
tst_status(1);$
