LIB "tst.lib"; tst_init();
LIB "decodegb.lib";
//correct 1 error in [15,7] binary code
int t=1; int q=16; int n=15; int redun=10;
ring r=(q,a),x,dp;
//generate random check matrix
matrix h=randomCheck(redun,n,1);
matrix g=dual_code(h);
matrix x[1][n-redun]=0,0,1,0,1,0,1;
matrix y[1][n]=encode(x,g);
print(y);
// find out the minimum distance of the code
list l=mindist(h);
//disturb with errors
"Correct ",(l[1]-1) div 2," errors";
matrix rec[1][n]=errorRand(y,(l[1]-1) div 2,1);
print(rec);
//let us decode
matrix dec_word=decode(h,rec);
print(dec_word);
tst_status(1);$
