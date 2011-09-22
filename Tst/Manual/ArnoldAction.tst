LIB "tst.lib"; tst_init();
LIB "qhmoduli.lib";
ring B   = 0,(x,y,z), ls;
poly f = -z5+y5+x2z+x2y;
def R = ArnoldAction(f);
setring R;
actionid;
stabid;
tst_status(1);$
