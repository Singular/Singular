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

// 761: rowShift, if betti[1,1] is 0
ring r2 = 0, x, dp;
module m = gen(1);
attrib(m, "isHomog", intvec(3));
print(betti(list(m), 0), "betti");

// 763: there may be 0 as generator
LIB "matrix.lib";
ring r3 = 0, (x,y,z), dp;
ideal i1 = x, y, z;
ideal i2 = 1;
resolution s1 = res(i1, 0);
resolution s2 = res(i2, 0);
list l1 = s1;
list l2 = s2;
print(betti(l1, 0), "betti");
print(betti(l2, 0), "betti");
list l_sum;
l_sum[3] = module(dsum(matrix(l1[3]), matrix(l2[1])));   // direct sum
l_sum[1] = l1[1];
l_sum[2] = module(l1[2][1], l1[2][2], l1[2][3], 0);
l_sum;

print(betti(l_sum, 0), "betti");

tst_status(1);$;


