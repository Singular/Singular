LIB "tst.lib";
tst_init();

// weightKB for a degree with no monomials at all

ring r = 0,(x,y),dp;
intvec w = 2,6;
weightKB(ideal(0),3,w); // bug
weightKB(ideal(0),4,w); // ok
weightKB(ideal(0),5,w); // bug
weightKB(ideal(0),6,w); // ok
weightKB(ideal(0),7,w); // ok

tst_status(1);$
