LIB "tst.lib"; tst_init();
LIB "mondromy.lib";
ring R=0,x,dp;
matrix U[2][2]=1,1+x,1+x2,1+x3;
list daU=detadj(U);
daU[1];
print(daU[2]);
tst_status(1);$
