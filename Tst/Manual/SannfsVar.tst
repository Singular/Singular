LIB "tst.lib"; tst_init();
LIB "dmodvar.lib";
ring R = 0,(x,y),Dp;
ideal F = x^3, y^5;
//ORD = "(a(1,1),a(1,1,1,1,1,1),dp)";
//eng = 0;
def A = SannfsVar(F);
setring A;
A;
LD;
tst_status(1);$
