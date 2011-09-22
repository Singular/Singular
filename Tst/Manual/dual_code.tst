LIB "tst.lib"; tst_init();
LIB "brnoeth.lib";
ring s=2,T,lp;
// here is the Hamming code of length 7 and dimension 3
matrix G[3][7]=1,0,1,0,1,0,1,0,1,1,0,0,1,1,0,0,0,1,1,1,1;
print(G);
matrix H=dual_code(G);
print(H);
tst_status(1);$
