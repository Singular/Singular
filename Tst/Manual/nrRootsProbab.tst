LIB "tst.lib"; tst_init();
LIB "rootsmr.lib";
ring r = 0,(x,y,z),lp;
ideal i = (x-1)*(x-2),(y-1)^3*(x-y),(z-1)*(z-2)*(z-3)^2;
nrRootsProbab(i);       //no of real roots (using internally std)
i = groebner(i);        //using the hilbert driven GB computation
int pr = printlevel;
printlevel = 2;
nrRootsProbab(i);
printlevel = pr;
tst_status(1);$
