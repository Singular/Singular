LIB "tst.lib";
tst_init();

// bug in solve
LIB"solve.lib";
ring r=0,y,dp;
ideal I=y6+46y4+529y2+12167;
option(prot);
// should give 6 solutions in C
solve(I);
ideal J=(y-1)*(y-2)*(y-3)*(y-4)*(y-5)*(y-6);
// should give 6 solutions in Z
solve(J);

tst_status(1);$
