LIB "tst.lib"; tst_init();
LIB "decodegb.lib";
ring r=2,x,dp;
matrix x[1][4]=1,0,1,0;
matrix g[4][7]=1,0,0,0,0,1,1,
0,1,0,0,1,0,1,
0,0,1,0,1,1,1,
0,0,0,1,1,1,0;
//encode x with the generator matrix g
print(encode(x,g));
tst_status(1);$
