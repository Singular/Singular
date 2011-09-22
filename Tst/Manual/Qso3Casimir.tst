LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def R = makeQso3(5);
setring R;
list C = Qso3Casimir(5);
C;
list Cnorm = Qso3Casimir(5,1);
Cnorm;
tst_status(1);$
