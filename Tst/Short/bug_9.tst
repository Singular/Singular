LIB "tst.lib";
tst_init();

ring r=0,(e(1..6),x,y,z),ws(-6,1,-13,-6,-20,-13,-2,-13,-6);
poly p1=10*e(5)*z^3-3*e(1)*x^2;
print(coef(p1,e(1)));
print(coef(p1,e(5)));

tst_status(1);$
