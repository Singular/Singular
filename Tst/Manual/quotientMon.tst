LIB "tst.lib"; tst_init();
LIB "monomialideal.lib";
ring R = 0,(w,x,y,z,t),lp;
ideal I = w^3*x*y,w*x*y*z*t,x^2*y^2*z^2,x^2*z^4*t^3,y^3*z;
ideal J = w*x, x^2, y*z*t, y^5*t;
quotientMon (I,J);
tst_status(1);$
