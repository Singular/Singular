LIB "tst.lib"; tst_init();
LIB "sing4ti2.lib";
ring r=0,(x,y,z),dp;
matrix M[2][3]=0,1,2,2,1,0;
markov4ti2(M);
matrix N[1][3]=1,2,1;
markov4ti2(N,1);
tst_status(1);$
