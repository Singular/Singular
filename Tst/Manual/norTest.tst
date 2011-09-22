LIB "tst.lib"; tst_init();
LIB "normal.lib";
int prl = printlevel;
printlevel = -1;
ring r = 0,(x,y),dp;
ideal i = (x-y^2)^2 - y*x^3;
list nor = normal(i);
norTest(i,nor);                //1,1,1 means that normal was correct
nor = normalC(i);
norTest(i,nor);                //1,1,1 means that normal was correct
ring s = 2,(x,y),dp;
ideal i = (x-y^2)^2 - y*x^3;
nor = normalP(i,"withRing");
norTest(i,nor);               //1,1,1 means that normalP was correct
printlevel = prl;
tst_status(1);$
