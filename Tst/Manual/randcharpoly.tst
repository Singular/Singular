LIB "tst.lib"; tst_init();
LIB "rootsmr.lib";
ring r = 0,(x,y,z),dp;
ideal i = (x-1)*(x-2),(y-1),(z-1)*(z-2)*(z-3)^2;
i = std(i);
ideal b = qbase(i);
poly p = randcharpoly(b,i);
p;
nrroots(p); // See nrroots in urrcount.lib
int pr = printlevel;
printlevel = pr+2;
p = randcharpoly(b,i,5);
nrroots(p);
printlevel = pr;
tst_status(1);$
