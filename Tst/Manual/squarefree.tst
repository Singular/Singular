LIB "tst.lib"; tst_init();
LIB "hnoether.lib";
ring exring=3,(x,y),dp;
squarefree((x3+y)^2);
squarefree((x+y)^3*(x-y)^2); // Warning: (x+y)^3 is lost
squarefree((x+y)^4*(x-y)^2); // result is (x+y)*(x-y)
tst_status(1);$
