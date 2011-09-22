LIB "tst.lib"; tst_init();
LIB "phindex.lib";
ring r=0,(x),ds;
matrix M[5][5]=0,0,0,1,0,0,1,0,0,-1,0,0,1,0,0,1,0,0,3,0,0,-1,0,0,1;
signatureL(M,1); //The rank of M is 3+1=4
matrix H[5][5]=0,-7,0,1,0,-7,1,0,0,-1,0,0,1,0,0,1,0,0,-3,5,0,-1,0,5,1;
signatureL(H);
tst_status(1);$
