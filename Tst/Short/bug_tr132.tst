LIB "tst.lib";
tst_init();

// redcue was not complete (strat->ak was 0)
ring r = 0,(x,y,z), (c, dp);
qring Q = std(ideal(var(1)**2, var(2)**2,
 var(3)**2));
reduce( maxideal(2) * gen(1), std(0));

tst_status(1);$
