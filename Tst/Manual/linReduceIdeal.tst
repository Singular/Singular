LIB "tst.lib"; tst_init();
LIB "bfun.lib";
ring r = 0,(x,y),dp;
ideal I = 3,x+9,y4+5x,2y4+7x+2;
linReduceIdeal(I);     // reduces tails
linReduceIdeal(I,0,0); // no reductions of tails
list l = linReduceIdeal(I,1); // reduces tails and shows reductions used
l;
module M = I;
matrix(l[1]) - M*l[2];
tst_status(1);$
