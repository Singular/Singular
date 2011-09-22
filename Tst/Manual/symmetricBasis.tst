LIB "tst.lib"; tst_init();
LIB "matrix.lib";
// basis of the 3-rd symmetricPower of a 4-dim vector space:
def R = symmetricBasis(4, 3, "@e"); setring R;
R;  // container ring:
symBasis; // symmetric basis:
tst_status(1);$
