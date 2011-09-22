LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def B = makeQso3(3);
// this algebra is a quantum deformation of U(so_3),
// where the quantum parameter is a 6th root of unity
setring B; B;
def Bopp = opposite(B);
setring Bopp;
Bopp;
def Bcheck = opposite(Bopp);
setring Bcheck; Bcheck;  // check that (B-opp)-opp = B
tst_status(1);$
