LIB "tst.lib";
tst_init();

// reduce was not complete (strat->ak was 0)
ring r = 0,(x,y,z), (c, dp);
qring Q = std(ideal(var(1)**2, var(2)**2,
 var(3)**2));
reduce( maxideal(2) * gen(1), std(0));

// std (compleReduce) was not complete (index bounds to small)
vector v = var(1)**2 + var(2)**2 + var(1)*var(2); v;
option(redTail); option(redSB);
std(v);

tst_status(1);$
