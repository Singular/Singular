LIB "tst.lib";
tst_init();

//bug in primdecGTZ() for block ordering
LIB"primdec.lib";
ring rng = 0,(xt, xd, xb ,xy, xg),( dp(2),dp(3) );
ideal I = 39*xt*xy+13*xd*xb, 32*xt^3*xb-35*xt*xy*xg^2;
list L2 = primdecSY(I);
L2;
testPrimary(L2, I);
list L1 = primdecGTZ(I);
L1;
testPrimary(L1, I);

tst_status(1);$
