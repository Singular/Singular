LIB "tst.lib"; tst_init();
LIB "resolve.lib";
ring R=0,(x,y),dp;
ideal J=x2-y3;
ideal C=x,y;
list blow=blowUp(J,C);
def Q=blow[1];
setring Q;
aS;
sT;
eD;
bM;
tst_status(1);$
