LIB "tst.lib"; tst_init();
LIB "decodegb.lib";
intvec v = option(get);
//correct 2 errors in [7,3] 8-ary code RS code
int t=2; int q=8; int n=7; int redun=4;
ring r=(q,a),x,dp;
matrix h_full=genMDSMat(n,a);
matrix h=submat(h_full,1..redun,1..n);
matrix g=dual_code(h);
matrix x[1][3]=0,0,1,0;
matrix y[1][7]=encode(x,g);
//disturb with 2 errors
matrix rec[1][7]=errorInsert(y,list(2,4),list(1,a));
//generate the system
def A=sysQE(h,rec,t);
setring A;
print(qe);
//let us decode
option(redSB);
ideal sys_qe=std(qe);
print(sys_qe);
option(set,v);
tst_status(1);$
