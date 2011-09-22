LIB "tst.lib"; tst_init();
ring R=0,(x,y),ds;
poly f=x5+x2y2+y5;
division(f,jacob(f)); // automatic conversion: poly -> ideal
division(f^2,jacob(f));
division(ideal(f^2),jacob(f),10);
tst_status(1);$
