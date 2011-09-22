LIB "tst.lib"; tst_init();
LIB "dmod.lib";
ring r = 0,(x,y),Dp;
poly F = x^4+y^5+x*y^4;
printlevel = 0;
def A = Sannfs(F);
setring A;
poly F = imap(r,F);
checkFactor(LD,F,20*s+31);     // -31/20 is not a root of bs
checkFactor(LD,F,20*s+11);     // -11/20 is a root of bs
checkFactor(LD,F,(20*s+11)^2); // the multiplicity of -11/20 is 1
tst_status(1);$
