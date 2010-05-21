LIB "tst.lib";
tst_init();

// hikbert driven std missed the comp 4 in the result
ring r = 0, x, (C, lp);
module M;
M[1]=gen(3);
M[2]=gen(4);
M[3]=gen(4)+x*gen(2)-x2*gen(1);
groebner(M);
option(prot);
stdhilb(M);

tst_status(1);$
