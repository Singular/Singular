LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def a = makeUsl2();       // U(sl_2) in e,f,h presentation
ring W0 = 0,(x,d),dp;
def W = Weyl();              // 1st Weyl algebra in x,d
def S = a+W;
setring S;
S;
tst_status(1);$
