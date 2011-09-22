LIB "tst.lib"; tst_init();
LIB "matrix.lib";
// basis of the 3-rd symmetricPower of a 4-dim vector space:
def r = exteriorBasis(4, 3, "@e"); setring r;
r; // container ring:
extBasis; // exterior basis:
tst_status(1);$
