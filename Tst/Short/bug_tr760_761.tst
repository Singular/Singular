LIB "tst.lib";
tst_init();

// 760: conversion list -> resolution must include attribute isHomog
ring r = 0, x, dp;
module m = 0;
attrib(m, "isHomog", intvec(3));
print(betti(list(m), 0), "betti");
attrib(m,"isHomog");
resolution s = list(m);
attrib(s);
attrib(s,"isHomog");
print(betti(s, 0), "betti");

// 761: rwowShift, if betti[1,1] is 0
ring r2 = 0, x, dp;
module m = gen(1);
attrib(m, "isHomog", intvec(3));
print(betti(list(m), 0), "betti");

tst_status(1);$;


