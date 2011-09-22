LIB "tst.lib"; tst_init();
LIB "latex.lib";
intmat m[3][4] = 9,2,4,5,2,5,-2,4,-6,10,-1,2,7;
opentex("exp001");
texobj("exp001","An intmat:  ",m);
closetex("exp001");
tex("exp001");
xdvi("exp001");
system("sh","rm exp001.*");
tst_status(1);$
