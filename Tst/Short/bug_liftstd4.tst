LIB "tst.lib";
tst_init();

// return type of liftsd with 4 args

ring R = 0,(x,y),lp;
matrix A[2][2] = x,0,0,y;
matrix T;
module S;
typeof(liftstd(module(A),T,S,"std"));
print(liftstd(module(A),T,S,"std"));
typeof(liftstd(ideal(x,y),T,S,"std"));

tst_status(1);$
