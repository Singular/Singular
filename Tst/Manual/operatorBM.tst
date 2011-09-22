LIB "tst.lib"; tst_init();
LIB "dmod.lib";
ring r = 0,(x,y,z),Dp;
poly F = x^3+y^3+z^3;
printlevel = 0;
def A = operatorBM(F);
setring A;
F; // the original polynomial itself
LD; // generic annihilator
LD0; // annihilator
bs; // normalized Bernstein poly
BS; // roots and multiplicities of the Bernstein poly
PS; // the operator, s.t. PS*F^{s+1} = bs*F^s mod LD
reduce(PS*F-bs,LD); // check the property of PS
tst_status(1);$
