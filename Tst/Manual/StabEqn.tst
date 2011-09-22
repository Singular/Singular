LIB "tst.lib"; tst_init();
LIB "qhmoduli.lib";
ring B = 0,(x,y,z), ls;
poly f = -z5+y5+x2z+x2y;
list stab = StabEqn(f);
def S1 = stab[1]; setring S1;  stabid;
def S2 = stab[2]; setring S2;  actionid;
tst_status(1);$
