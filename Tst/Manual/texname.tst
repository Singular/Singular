LIB "tst.lib"; tst_init();
LIB "latex.lib";
ring r = 0,(x,y),lp;
poly f = 3xy4 + 2xy2 + x5y3 + x + y6;
texname("","{f(10)}");
texname("","f(10) =");
texname("","n1");
texname("","T1_12");
texname("","g'_11");
texname("","f23");
texname("","M[2,3]");
texname("","A(0,3);");
texname("","E~(3)");
tst_status(1);$
