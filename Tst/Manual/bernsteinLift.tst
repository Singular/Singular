LIB "tst.lib"; tst_init();
LIB "dmod.lib";
ring r = 0,(x,y,z),Dp;
poly F = x^3+y^3+z^3;
printlevel = 0;
def A = Sannfs(F);   setring A;
LD;
poly F = imap(r,F);
list L  = bernsteinLift(LD,F); L;
poly bs = fl2poly(L,"s"); bs; // the candidate for Bernstein-Sato polynomial
tst_status(1);$
