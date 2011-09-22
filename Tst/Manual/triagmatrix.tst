LIB "tst.lib"; tst_init();
LIB "random.lib";
ring r = 0,(a,b,c,d),dp;
// sparse triagonal matrix of sparse polys of degree <=2:
print(triagmatrix(3,4,2));"";
// dense triagonal matrix of sparse linear forms:
print(triagmatrix(3,3,1,1,0,55,9));
tst_status(1);$
