LIB "tst.lib";
tst_init();

ring r;
matrix m[2][2]=1,2,3,4;
system("rref",m);
system("rref",smatrix(m));
m=1,2,0,4; // 0-entry
system("rref",m);
system("rref",smatrix(m));
m=1,2,2,4; // rk 1
system("rref",m);
system("rref",smatrix(m));

ring R=QQ,x,dp;
matrix m[2][2]=1,2,3,4;
system("rref",m);
system("rref",smatrix(m));
m=1,2,0,4;
system("rref",m);
system("rref",smatrix(m));
m=1,2,2,4;
system("rref",m);
system("rref",smatrix(m));


tst_status(1);$
