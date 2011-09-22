LIB "tst.lib"; tst_init();
LIB "rootsmr.lib";
ring r = 0,(x,y,z),dp;
ideal i = (x-1)*(x-2),(y-1),(z+5); // V(I) = {(1,1,-5),(2,1,-5)}
i = std(i);
ideal b = qbase(i);
poly f = x+y+z;
list psums = list(-2-3,4+9); // f evaluated at V(I) gives {-3,-2}
list l = powersums(f,b,i);
psums;
l;
tst_status(1);$
