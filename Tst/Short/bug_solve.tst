LIB "tst.lib";
tst_init();

// solve for rings with complex cf.
LIB "solve.lib";
ring r = complex, (x,y), dp;
ideal I = ((x-5)*y+16)/32, y^2-x^3-5*x-10;
def R=solve(I);
setring R; SOL;
R;

tst_status(1);$
