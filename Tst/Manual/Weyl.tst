LIB "tst.lib"; tst_init();
LIB "nctools.lib";
ring A1=0,(x(1..2),d(1..2)),dp;
def S=Weyl();
setring S;  S;
kill A1,S;
ring B1=0,(x1,d1,x2,d2),dp;
def S=Weyl(1);
setring S;  S;
tst_status(1);$
