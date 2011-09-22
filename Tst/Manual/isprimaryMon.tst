LIB "tst.lib"; tst_init();
LIB "monomialideal.lib";
ring R = 0,(w,x,y,z,t),lp;
ideal I = w^4,x^3,z^2,t^5,x*t,w*x^2*z;
isprimaryMon (I);
ideal J = w^4,x^3,z^2,t^5,x*t,w*x^2*z,y^3*t^3;
isprimaryMon (J);
tst_status(1);$
