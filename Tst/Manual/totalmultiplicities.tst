LIB "tst.lib"; tst_init();
LIB "alexpoly.lib";
ring r=0,(x,y),ls;
poly f1=(y2-x3)^2-4x5y-x7;
poly f2=y2-x3;
poly f3=y3-x2;
totalmultiplicities(f1*f2*f3);
tst_status(1);$
