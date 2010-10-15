LIB "tst.lib";
tst_init();

// we should see error messages in all cases:
ring RING=0,(X,Y),dp;
lift(ideal(X),Y);
lift(ideal(0),Y);
lift(module(gen(10)),gen(11));
lift(module(gen(10)),gen(12));

tst_status(1);$
