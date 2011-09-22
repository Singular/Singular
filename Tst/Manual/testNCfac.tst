LIB "tst.lib"; tst_init();
LIB "ncfactor.lib";
ring r = 0,(x,y),dp;
def R = nc_algebra(1,1);
setring R;
poly h = (x^2*y^2+1)*(x^2);
def t1 = facFirstWeyl(h);
//fist a correct list
testNCfac(t1);
//now a correct list with the factorized polynomial
testNCfac(t1,h);
//now we put in an incorrect list without a polynomial
t1[3][3] = y;
testNCfac(t1);
// take h as additional input
testNCfac(t1,h);
// take h as additional input and output list of differences
testNCfac(t1,h,1);
tst_status(1);$
