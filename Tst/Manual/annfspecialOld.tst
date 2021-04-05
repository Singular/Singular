LIB "tst.lib"; tst_init();
LIB "dmod.lib";
ring r = 0,(x,y),dp;
poly F = x3-y2;
def  B = annfs(F);  setring B;
minIntRoot(BS[1],0);
// So, the minimal integer root is -1
setring r;
def  A = SannfsBM(F);
setring A;
poly F = x3-y2;
annfspecialOld(LD,F,-1,3/4); // generic root
annfspecialOld(LD,F,-1,-2);  // integer but still generic root
annfspecialOld(LD,F,-1,1);   // exceptional root
tst_status(1);$
