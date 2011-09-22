LIB "tst.lib"; tst_init();
LIB "monomialideal.lib";
ring R = 0,(w,x,y,z,t),lp;
ideal I = w^4,x^3,z^2,t^5;
isirreducibleMon (I);
ideal J = w^4*x,x^3,z^2,t^5;
isirreducibleMon (J);
tst_status(1);$
