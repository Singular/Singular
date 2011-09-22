LIB "tst.lib"; tst_init();
LIB "homolog.lib";
ring R=0,(x,y,z),dp;
matrix M[2][3];     // the presentation matrix
M=x-1,y-1,z,y-1,x-2,x;
ideal I=fitting(M,0); // 0-th Fitting ideal of coker(M)
qring Q=I;
matrix M=fetch(R,M);
isLocallyFree(M,1); // as R/I-module, coker(M) is locally free of rk 1
isLocallyFree(M,0);
tst_status(1);$
