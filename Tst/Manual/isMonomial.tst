LIB "tst.lib"; tst_init();
LIB "monomialideal.lib";
ring R = 0,(w,x,y,z,t),lp;
ideal I = w^3*x*y, w^2*x^2*y^2*z^2 - y^3*z+x^4*z^4*t^4, w*x*y*z*t - w*x^6*y^5*z^4, x^2*z^4*t^3 , w^6*y^4*z^2 + x^2*y^2*z^2;
isMonomial(I);
ideal J = w^3*x*y + x^3*y^9*t^3, w^2*x^2*y^2*z^2 - y^3*z, w*x*y*z*t - w*x^6*y^5*z^4, x^2*z^4*t^3 + y^4*z^4*t^4, w^6*y^4*z^2 + x^2*y^2*z^2;
isMonomial(J);
tst_status(1);$
