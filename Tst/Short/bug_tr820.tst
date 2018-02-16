LIB "tst.lib";
tst_init();

// results must be equal (as ideals)
LIB "paraplanecurves.lib";
ring R1 = 0,(x,y,z),dp;
poly f=x^3*y - x^2*z^2 + y^2*z^2 + y*z^3;
adjointIdeal(f,1);
ring R2 = 0,(y,z,x),dp;
poly f=x^3*y - x^2*z^2 + y^2*z^2 + y*z^3;
adjointIdeal(f,1);

tst_status(1);$
