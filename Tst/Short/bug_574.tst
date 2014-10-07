LIB "tst.lib";
tst_init();

LIB("primdecint.lib");
ring rng = integer,(xk,xe),dp;

ideal I = 9*xk^3+8*xk+5,
2*xk*xe^2-8*xk^2-11*xe,
xk^2*xe+9;

def MMM=module(I);

def pdec = primdecZM(MMM);// how to check for correctness of pdec??


tst_status(1); $;

