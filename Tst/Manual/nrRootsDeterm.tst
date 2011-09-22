LIB "tst.lib"; tst_init();
LIB "rootsmr.lib";
ring r = 0,(x,y,z),lp;
ideal I = (x-1)*(x-2),(y-1),(z-1)*(z-2)*(z-3)^2;
nrRootsDeterm(I);       //no of real roots (using internally std)
I = groebner(I);        //using the hilbert driven GB computation
int pr = printlevel;
printlevel = 2;
nrRootsDeterm(I);
printlevel = pr;
tst_status(1);$
