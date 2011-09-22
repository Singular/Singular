LIB "tst.lib"; tst_init();
LIB "normaliz.lib";
ring R=0,(x,y,z,t),dp;
ideal I=x^2,y^2,z^3;
list l=intclMonIdeal(I);
l[1]; // integral closure of I
l[2];  // monomials generating the integral closure of the Rees algebra
tst_status(1);$
