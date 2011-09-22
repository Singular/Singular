LIB "tst.lib"; tst_init();
LIB "monomialideal.lib";
ring R = 0,(w,x,y,z,t),lp;
ideal I =  w*x, x^2, y*z*t, y^5*t;
poly f =  3*x^2*y + 6*t^5*z*y^6 - 4*x^2 + 8*w*x^5*y^6 - 10*y^10*t^10;
membershipMon(f,I);
poly g = 3*w^2*t^3 - 4*y^3*z*t^3 - 2*x^2*y^5*t + 4*x*y^3;
membershipMon(g,I);
tst_status(1);$
