LIB "tst.lib"; tst_init();
LIB "decodegb.lib";
int q=16; int n=15;
ring r=(q,a),x,dp;
//generate an MDS (Vandermonde) matrix
matrix h_full=genMDSMat(n,a);
print(h_full);
tst_status(1);$
