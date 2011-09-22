LIB "tst.lib"; tst_init();
LIB "ncfactor.lib";
ring r = 0,(x,y,z),dp;
matrix D[3][3]; D[1,3]=-1;
def R = nc_algebra(1,D); // x,z generate Weyl subalgebra
setring R;
poly h = (x^2*z^2+x)*x;
list fact1 = facSubWeyl(h,x,z);
// compare with facFirstWeyl:
ring s = 0,(z,x),dp;
def S = nc_algebra(1,1); setring S;
poly h = (x^2*z^2+x)*x;
list fact2 = facFirstWeyl(h);
map F = R,x,0,z;
list fact1 = F(fact1); // it is identical to list fact2
testNCfac(fact1); // check the correctness again
tst_status(1);$
