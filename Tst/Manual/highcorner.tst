LIB "tst.lib"; tst_init();
ring r=0,(x,y),ds;
ideal i=x3,x2y,y3;
highcorner(std(i));
highcorner(std(ideal(1)));
tst_status(1);$
