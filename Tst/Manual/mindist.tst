LIB "tst.lib"; tst_init();
LIB "decodegb.lib";
//determine a minimum distance for a [7,3] binary code
int q=8; int n=7; int redun=4; int t=redun+1;
ring r=(q,a),x,dp;
//generate random check matrix
matrix h=randomCheck(redun,n,1);
print(h);
int l=mindist(h);
l;
tst_status(1);$
