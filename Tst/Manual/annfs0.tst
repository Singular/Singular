LIB "tst.lib"; tst_init();
LIB "dmod.lib";
ring r = 0,(x,y,z),Dp;
poly F = x^3+y^3+z^3;
printlevel = 0;
def A = SannfsBM(F);   setring A;
// alternatively, one can use SannfsOT or SannfsLOT
LD;
poly F = imap(r,F);
def B  = annfs0(LD,F);  setring B;
LD;
BS;
tst_status(1);$
