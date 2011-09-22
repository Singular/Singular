LIB "tst.lib"; tst_init();
LIB "decodegb.lib";
ring r=2,x,dp;
matrix x[1][4]=1,0,1,0;
matrix g[4][7]=1,0,0,0,0,1,1,
0,1,0,0,1,0,1,
0,0,1,0,1,1,1,
0,0,0,1,1,1,0;
//encode x with the generator matrix g
matrix c=encode(x,g);
// disturb
c[1,3]=0;
//compute syndrome
//corresponding check matrix
matrix check[3][7]=1,0,0,1,1,0,1,0,1,0,1,0,1,1,0,0,1,0,1,1,1;
print(syndrome(check,c));
c[1,3]=1;
//now c is a codeword
print(syndrome(check,c));
tst_status(1);$
