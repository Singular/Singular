LIB "tst.lib"; tst_init();
LIB "monomialideal.lib";
ring R = 0,(w,x,y,z,t),lp;
ideal I = w^3*x*y,w*x*y*z*t,x^2*y^2*z^2,x^2*z^4*t^3,y^3*z;
dimMon (I);
ideal J = w^4,x^3,y^4,z^2,t^6,w^2*x^2*y,w*z*t^4,x^2*y^3,z*t^5;
dimMon (J);
tst_status(1);$
