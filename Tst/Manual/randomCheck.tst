LIB "tst.lib"; tst_init();
LIB "decodegb.lib";
int redun=5; int n=15;
ring r=2,x,dp;
//generate random check matrix for a [15,5] binary code
matrix h=randomCheck(redun,n,1);
print(h);
//corresponding generator matrix
matrix g=dual_code(h);
print(g);
tst_status(1);$
