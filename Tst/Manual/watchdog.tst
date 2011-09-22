LIB "tst.lib"; tst_init();
LIB "general.lib";
ring r=0,(x,y,z),dp;
poly f=x^30+y^30;
watchdog(1,"factorize(eval("+string(f)+"))");
watchdog(100,"factorize(eval("+string(f)+"))");
tst_status(1);$
