LIB "tst.lib"; tst_init();
LIB "qhmoduli.lib";
ring B   = 0,(x,y,z), ls;
ideal I = x2,y3,z6;
intvec w = 3,2,1;
list stab = StabEqnId(I, w);
def S1 = stab[1]; setring S1;  stabid;
def S2 = stab[2]; setring S2;  actionid;
tst_status(1);$
