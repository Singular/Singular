LIB "tst.lib";
tst_init();

// used to have o polymial HEcke,
// and therefore, reduce all vectors to 0
ring r=0,(A(1..5)),(c,ds);
reduce(A(1)^3*gen(1),maxideal(4));
reduce(A(1)^2*gen(1),maxideal(4));

tst_status(1);$
